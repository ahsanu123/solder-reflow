#ifndef LVGL_NATIVE
#define LVGL_NATIVE

#include <stdio.h>
#include <sys/lock.h>
#include <sys/param.h>
#include <unistd.h>

#include "anim/lv_example_anim.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "get_started/lv_example_get_started.h"
#include "hal/spi_types.h"
#include "layouts/flex/lv_example_flex.h"
#include "lvgl.h"
#include "misc/lv_types.h"
#include "widgets/button/lv_button.h"

static SemaphoreHandle_t lvgl_api_lock;
#define LVGL_TICK_PERIOD_MS 2
#define LVGL_TASK_PRIORITY 2
#define LVGL_TASK_STACK_SIZE (4 * 1024)
#define TAG "NATIVE"

#define LCD_SPI_RST gpio_num_t::GPIO_NUM_32
#define LCD_SPI_DC gpio_num_t::GPIO_NUM_33
#define LCD_SPI_CS gpio_num_t::GPIO_NUM_2
#define LCD_SPI_CLK gpio_num_t::GPIO_NUM_18
#define LCD_SPI_D gpio_num_t::GPIO_NUM_23
#define LCD_HEIGHT 320 // 240
#define LCD_WIDTH 212  // 240
#define LCD_SPI_HOST SPI2_HOST

#define LCD_PIXEL_CLOCK (20 * 1000 * 1000)
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8
#define LCD_BUF_HEIGHT 50
#define LCD_BIT_PER_PIXEL 16

static bool example_notify_lvgl_flush_ready(
  esp_lcd_panel_io_handle_t      panel_io,
  esp_lcd_panel_io_event_data_t *edata,
  void                          *user_ctx
) {
  lv_display_t *disp = (lv_display_t *)user_ctx;
  lv_display_flush_ready(disp);
  return false;
}

static void example_lvgl_port_update_callback(lv_display_t *disp) {
  esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
  lv_display_rotation_t  rotation     = lv_display_get_rotation(disp);

  switch (rotation) {
    case LV_DISPLAY_ROTATION_0:
      // Rotate LCD  display
      esp_lcd_panel_swap_xy(panel_handle, false);
      esp_lcd_panel_mirror(panel_handle, true, false);
      break;
    case LV_DISPLAY_ROTATION_90:
      // Rotate LCD  display
      esp_lcd_panel_swap_xy(panel_handle, true);
      esp_lcd_panel_mirror(panel_handle, true, true);
      break;
    case LV_DISPLAY_ROTATION_180:
      // Rotate LCD  display
      esp_lcd_panel_swap_xy(panel_handle, false);
      esp_lcd_panel_mirror(panel_handle, false, true);
      break;
    case LV_DISPLAY_ROTATION_270:
      // Rotate LCD display
      esp_lcd_panel_swap_xy(panel_handle, true);
      esp_lcd_panel_mirror(panel_handle, false, false);
      break;
  }
}

static void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  example_lvgl_port_update_callback(disp);
  esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
  int                    offsetx1     = area->x1;
  int                    offsetx2     = area->x2;
  int                    offsety1     = area->y1;
  int                    offsety2     = area->y2;
  lv_draw_sw_rgb565_swap(px_map, (offsetx2 + 1 - offsetx1) * (offsety2 + 1 - offsety1));
  esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

static void example_increase_lvgl_tick(void *arg) { lv_tick_inc(LVGL_TICK_PERIOD_MS); }

static void example_lvgl_port_task(void *arg) {
  ESP_LOGI(
    TAG,
    "Starting LVGL "
    "task"
  );
  uint32_t time_till_next_ms = 0;
  uint32_t time_threshold_ms = 1000 / CONFIG_FREERTOS_HZ;
  while (1) {

    xSemaphoreTake(lvgl_api_lock, pdMS_TO_TICKS(100));
    time_till_next_ms = lv_timer_handler();
    xSemaphoreGive(&lvgl_api_lock);

    time_till_next_ms = MAX(time_till_next_ms, time_threshold_ms);
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

void *spi_bus_dma_memory_alloc(spi_host_device_t host_id, size_t size, uint32_t extra_heap_caps) {
  (void)host_id; // remain for extendability
  ESP_RETURN_ON_FALSE((extra_heap_caps & MALLOC_CAP_SPIRAM) == 0, NULL, "SPI", "external memory is not supported now");

  size_t dma_requir =
    16; // TODO: IDF-10111, using max alignment temp, refactor to "gdma_get_alignment_constraints" instead
  return heap_caps_aligned_calloc(dma_requir, 1, size, extra_heap_caps | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

void nativeDemoLVGL() {
  lvgl_api_lock                     = xSemaphoreCreateBinary();

  spi_bus_config_t lcdSpi_busConfig = {
    .mosi_io_num     = LCD_SPI_D,
    .miso_io_num     = GPIO_NUM_NC,
    .sclk_io_num     = LCD_SPI_CLK,
    .quadwp_io_num   = GPIO_NUM_NC,
    .quadhd_io_num   = GPIO_NUM_NC,
    .max_transfer_sz = LCD_HEIGHT * 80 * sizeof(uint16_t)
  };

  ESP_ERROR_CHECK(spi_bus_initialize(LCD_SPI_HOST, &lcdSpi_busConfig, SPI_DMA_CH_AUTO));

  esp_lcd_panel_io_handle_t     lcdPanelIO_handle;
  esp_lcd_panel_io_spi_config_t lcdPanel_spiConfig = {
    .cs_gpio_num       = LCD_SPI_CS,
    .dc_gpio_num       = LCD_SPI_DC,
    .spi_mode          = 0,
    .pclk_hz           = LCD_PIXEL_CLOCK,
    .trans_queue_depth = 10,
    .lcd_cmd_bits      = LCD_CMD_BITS,
    .lcd_param_bits    = LCD_PARAM_BITS,
  };

  ESP_ERROR_CHECK(
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &lcdPanel_spiConfig, &lcdPanelIO_handle)
  );

  esp_lcd_panel_handle_t     lcdPanel_handle;
  esp_lcd_panel_dev_config_t lcdPanel_config = {
    .reset_gpio_num = LCD_SPI_RST,
    .rgb_ele_order  = lcd_rgb_element_order_t::LCD_RGB_ELEMENT_ORDER_BGR,
    .bits_per_pixel = LCD_BIT_PER_PIXEL
  };

  ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(lcdPanelIO_handle, &lcdPanel_config, &lcdPanel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(lcdPanel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(lcdPanel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcdPanel_handle, true));

  lv_init();
  lv_display_t *display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);

  // alloc draw buffers used by LVGL
  // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
  size_t draw_buffer_sz = LCD_HEIGHT * LCD_BUF_HEIGHT * sizeof(lv_color16_t);

  void  *buf1           = spi_bus_dma_memory_alloc(LCD_SPI_HOST, draw_buffer_sz, 0);
  assert(buf1);
  void *buf2 = spi_bus_dma_memory_alloc(LCD_SPI_HOST, draw_buffer_sz, 0);
  assert(buf2);

  // initialize LVGL draw buffers
  lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
  // associate the mipi panel handle to the display
  lv_display_set_user_data(display, lcdPanel_handle);
  // set color depth
  lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
  // set the callback which can copy the rendered image to an area of the display
  lv_display_set_flush_cb(display, example_lvgl_flush_cb);

  ESP_LOGI(TAG, "Install LVGL tick timer");
  // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
  const esp_timer_create_args_t lvgl_tick_timer_args = {.callback = &example_increase_lvgl_tick, .name = "lvgl_tick"};
  esp_timer_handle_t            lvgl_tick_timer      = NULL;
  ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

  ESP_LOGI(TAG, "Register io panel event callback for LVGL flush ready notification");
  const esp_lcd_panel_io_callbacks_t cbs = {
    .on_color_trans_done = example_notify_lvgl_flush_ready,
  };

  ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(lcdPanelIO_handle, &cbs, display));

  ESP_LOGI(TAG, "Create LVGL task");
  xTaskCreate(example_lvgl_port_task, "LVGL", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);

  // Lock the mutex due to the LVGL APIs are not thread-safe
  xSemaphoreTake(lvgl_api_lock, pdMS_TO_TICKS(100));
  lv_example_get_started_1();
  xSemaphoreGive(&lvgl_api_lock);
}

#endif
