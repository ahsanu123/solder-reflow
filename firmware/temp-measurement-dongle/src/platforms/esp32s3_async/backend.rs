use alloc::rc::Rc;
use core::time::Duration;
use defmt::info;
use embassy_time::Instant;
use slint::{
    PlatformError,
    platform::{
        Platform, WindowAdapter,
        software_renderer::{self, MinimalSoftwareWindow},
    },
};

pub const DISPLAY_WIDTH: usize = 320;
pub const DISPLAY_HEIGHT: usize = 172;

pub type TargetPixelType = software_renderer::Rgb565Pixel;

pub struct Esp32AsyncBackend {
    window: Rc<MinimalSoftwareWindow>,
}

impl Esp32AsyncBackend {
    pub fn new(window: Rc<MinimalSoftwareWindow>) -> Self {
        Self { window }
    }
}

impl Platform for Esp32AsyncBackend {
    fn create_window_adapter(&self) -> Result<Rc<dyn WindowAdapter>, PlatformError> {
        let window = self.window.clone();
        info!("create_window_adapter called");
        Ok(window)
    }
    fn duration_since_start(&self) -> Duration {
        Instant::now().duration_since(Instant::from_secs(0)).into()
    }
}
