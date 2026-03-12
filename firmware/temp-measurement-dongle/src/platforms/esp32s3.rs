use crate::platforms::esp32s3::esp_backend::EspBackend;
use alloc::boxed::Box;
use core::cell::RefCell;
use core::default::Default;
use defmt::info;
use esp_alloc as _;
use esp_hal::clock::CpuClock;
use {esp_backtrace as _, esp_println as _};

pub mod draw_buffer;
pub mod esp_backend;

// TODO: try to understand how this platform integrate with slint
// look at this docs: https://github.com/slint-ui/slint/blob/master/examples/mcu-board-support/README.md

/// Initializes the heap and sets the Slint platform.
pub fn init() {
    // Initialize peripherals first.
    let peripherals = esp_hal::init(esp_hal::Config::default().with_cpu_clock(CpuClock::_240MHz));
    // init_logger_from_env();
    info!("Peripherals initialized");

    // Initialize the PSRAM allocator.
    // esp_alloc::psram_allocator!(peripherals.PSRAM, esp_hal::psram);
    esp_alloc::heap_allocator!(#[esp_hal::ram(reclaimed)] size: 73744);

    // Create an EspBackend that now owns the peripherals.
    slint::platform::set_platform(Box::new(EspBackend {
        peripherals: RefCell::new(Some(peripherals)),
        window: RefCell::new(None),
    }))
    .expect("backend already initialized");
}
