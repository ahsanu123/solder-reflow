#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

use alloc::boxed::Box;
use defmt::info;
use defmt::println;
use embassy_executor::Spawner;
use embassy_futures::select::{Either4, select4};
use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::pubsub::PubSubChannel;
use embassy_sync::pubsub::WaitResult;
use embassy_time::Timer;
use embedded_hal_bus::spi::ExclusiveDevice;
use esp_hal::clock::CpuClock;
use esp_hal::delay::Delay;
use esp_hal::dma::{DmaRxBuf, DmaTxBuf};
use esp_hal::dma_buffers;
use esp_hal::gpio::{Input, InputConfig, Level, Output, OutputConfig, Pull};
use esp_hal::spi::Mode;
use esp_hal::spi::master::Config;
use esp_hal::spi::master::Spi;
use esp_hal::time::Rate;
use esp_hal::timer::timg::TimerGroup;
use mipidsi::options::{ColorOrder, Orientation, Rotation};
use slint::ComponentHandle;
use slint::platform::Key;
use slint::platform::WindowEvent;
use slint::platform::software_renderer::{MinimalSoftwareWindow, RepaintBufferType};
use static_cell::StaticCell;
use tmd::platforms::esp32s3_async::backend::Esp32AsyncBackend;
use tmd::platforms::esp32s3_async::draw_buffer::DrawBuffer;
use tmd_ui::TmdApp;
use {esp_backtrace as _, esp_println as _};

extern crate alloc;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

const MY_TASK_POOL_SIZE: usize = 2;

static INTERFACE_BUFFER: StaticCell<[u8; 312]> = StaticCell::new();

static DISPLAY_HEIGHT: usize = 172;
static DISPLAY_WIDTH: usize = 320;

#[derive(Clone)]
pub enum KeyEvent {
    Up,
    Right,
    Down,
    Left,
}

static PUBSUB_INPUT: PubSubChannel<CriticalSectionRawMutex, KeyEvent, 4, 2, 1> =
    PubSubChannel::<CriticalSectionRawMutex, KeyEvent, 4, 2, 1>::new();

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[esp_rtos::main]
async fn main(spawner: Spawner) {
    esp_alloc::heap_allocator!(#[esp_hal::ram(reclaimed)] size: 73744);

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::_80MHz);
    let peripherals = esp_hal::init(config);

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let input1 = peripherals.GPIO17;
    let input2 = peripherals.GPIO18;
    let input3 = peripherals.GPIO19;
    let input4 = peripherals.GPIO20;

    let lcd_dc = peripherals.GPIO15;
    let lcd_cs = peripherals.GPIO10;
    let lcd_rst = peripherals.GPIO14;
    let lcd_blk = peripherals.GPIO16;

    let scl_mosi = peripherals.GPIO13;
    let scl_sck = peripherals.GPIO12;
    let scl_miso = peripherals.GPIO11;

    let dma_channel = peripherals.DMA_CH0;

    let input1 = Input::new(input1, InputConfig::default().with_pull(Pull::Up));
    let input2 = Input::new(input2, InputConfig::default().with_pull(Pull::Up));
    let input3 = Input::new(input3, InputConfig::default().with_pull(Pull::Up));
    let input4 = Input::new(input4, InputConfig::default().with_pull(Pull::Up));

    let lcd_cs = Output::new(lcd_cs, Level::Low, OutputConfig::default());
    let lcd_dc = Output::new(lcd_dc, Level::Low, OutputConfig::default());
    let lcd_rst = Output::new(lcd_rst, Level::Low, OutputConfig::default());
    let mut lcd_blk = Output::new(lcd_blk, Level::Low, OutputConfig::default());

    let (rx_buffer, rx_descriptors, tx_buffer, tx_descriptors) = dma_buffers!(2048);
    let dma_rx_buf = DmaRxBuf::new(rx_descriptors, rx_buffer).unwrap();
    let dma_tx_buf = DmaTxBuf::new(tx_descriptors, tx_buffer).unwrap();

    let spi_bus = Spi::new(
        peripherals.SPI2,
        Config::default()
            .with_frequency(Rate::from_mhz(60))
            .with_mode(Mode::_0),
    )
    .unwrap()
    .with_sck(scl_sck)
    .with_mosi(scl_mosi)
    .with_miso(scl_miso)
    .with_dma(dma_channel)
    .with_buffers(dma_rx_buf, dma_tx_buf);

    let interface_buffer = INTERFACE_BUFFER.init([0u8; 312]);

    let mut delay = Delay::new();
    let spi_device = ExclusiveDevice::new(spi_bus, lcd_cs, delay).unwrap();

    let di = mipidsi::interface::SpiInterface::new(spi_device, lcd_dc, interface_buffer);

    let st7789 = mipidsi::Builder::new(tmd::display_models::st7789_modified::ST7789, di)
        .display_size(DISPLAY_WIDTH as u16, DISPLAY_HEIGHT as u16)
        .orientation(Orientation::new().rotate(Rotation::Deg270))
        .color_order(ColorOrder::Rgb)
        .display_offset(34, 340)
        .invert_colors(mipidsi::options::ColorInversion::Inverted)
        .reset_pin(lcd_rst)
        .init(&mut delay)
        .expect("fail to build display st7789");

    let mut draw_buffer = DrawBuffer::new(st7789);

    let window = MinimalSoftwareWindow::new(RepaintBufferType::ReusedBuffer);
    window.set_size(slint::PhysicalSize::new(
        DISPLAY_WIDTH as u32,
        DISPLAY_HEIGHT as u32,
    ));

    let backend = Box::new(Esp32AsyncBackend::new(window.clone()));
    slint::platform::set_platform(backend).expect("fail when set platform");

    info!("slint gui setup complete");

    lcd_blk.set_high();

    let tmd_app = TmdApp::new().expect("cant create Tmb Application");
    tmd_app.show().expect("unable to show tmb_app");

    spawner
        .spawn(button_task(input1, input2, input3, input4))
        .expect("fail to spawn button_task");

    let mut sub = PUBSUB_INPUT
        .subscriber()
        .expect("fail to make subscribtion");

    loop {
        slint::platform::update_timers_and_animations();

        window.draw_if_needed(|renderer| {
            info!("render by line!");
            renderer.render_by_line(&mut draw_buffer);
        });

        // TODO: simplify this
        if let Some(wait_result) = sub.try_next_message() {
            match wait_result {
                WaitResult::Lagged(amount) => println!("lag {} message", amount),

                WaitResult::Message(key) => match key {
                    KeyEvent::Up => {
                        println!("got up message");
                        window.dispatch_event(WindowEvent::KeyPressed {
                            text: Key::UpArrow.into(),
                        });
                    }
                    KeyEvent::Right => {
                        println!("got right message");
                        window.dispatch_event(WindowEvent::KeyPressed {
                            text: Key::RightArrow.into(),
                        });
                    }
                    KeyEvent::Down => {
                        println!("got down message");
                        window.dispatch_event(WindowEvent::KeyPressed {
                            text: Key::DownArrow.into(),
                        });
                    }
                    KeyEvent::Left => {
                        println!("got left message");
                        window.dispatch_event(WindowEvent::KeyPressed {
                            text: Key::LeftArrow.into(),
                        });
                    }
                },
            };
        }

        Timer::after_millis(50).await;
    }
}

#[embassy_executor::task(pool_size = MY_TASK_POOL_SIZE)]
async fn button_task(
    mut in1: Input<'static>,
    mut in2: Input<'static>,
    mut in3: Input<'static>,
    mut in4: Input<'static>,
) {
    let publisher = PUBSUB_INPUT.publisher().expect("fail to create publisher");

    loop {
        let event = select4(
            in1.wait_for_falling_edge(),
            in2.wait_for_falling_edge(),
            in3.wait_for_falling_edge(),
            in4.wait_for_falling_edge(),
        )
        .await;

        match event {
            Either4::First(_) => publisher.publish(KeyEvent::Up).await,
            Either4::Second(_) => publisher.publish(KeyEvent::Right).await,
            Either4::Third(_) => publisher.publish(KeyEvent::Down).await,
            Either4::Fourth(_) => publisher.publish(KeyEvent::Left).await,
        };
    }
}
