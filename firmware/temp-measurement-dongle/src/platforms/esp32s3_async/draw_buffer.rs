use core::convert::Infallible;
use embedded_hal::digital::OutputPin;
use mipidsi::{Display, interface::Interface, models::ST7789};
use slint::platform::software_renderer::{LineBufferProvider, Rgb565Pixel};
use {esp_backtrace as _, esp_println as _};

/// Provides a draw buffer for the MinimalSoftwareWindow renderer.
pub struct DrawBuffer<DISPLAY> {
    pub display: DISPLAY,
    pub buffer: &'static mut [Rgb565Pixel],
}

impl<DI: Interface<Word = u8>, RST: OutputPin<Error = Infallible>> LineBufferProvider
    for &mut DrawBuffer<Display<DI, ST7789, RST>>
{
    type TargetPixel = Rgb565Pixel;

    fn process_line(
        &mut self,
        line: usize,
        range: core::ops::Range<usize>,
        render_fn: impl FnOnce(&mut [Rgb565Pixel]),
    ) {
        let buffer = &mut self.buffer[range.clone()];
        render_fn(buffer);

        // Update the display with the rendered line.
        self.display
            .set_pixels(
                range.start as u16,
                line as u16,
                range.end as u16,
                line as u16,
                buffer
                    .iter()
                    .map(|x| embedded_graphics_core::pixelcolor::raw::RawU16::new(x.0).into()),
            )
            .unwrap();
    }
}
