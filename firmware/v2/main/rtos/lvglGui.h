#ifndef LVGL_GUI
#define LVGL_GUI

#include "core/lv_obj_pos.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_types.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_lvgl_port_disp.h"
#include "hal/spi_types.h"
#include "lv_api_map_v8.h"
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "misc/lv_types.h"
#include "portmacro.h"
#include "soc/gpio_num.h"
#include <cstdint>
#include <lvgl.h>
#include <lvgl__lvgl/src/display/lv_display_private.h>

#define MS_DELAY(TIME) TIME / portTICK_PERIOD_MS

#define LCD_SPI_RST gpio_num_t::GPIO_NUM_32
#define LCD_SPI_DC gpio_num_t::GPIO_NUM_33
#define LCD_SPI_CS gpio_num_t::GPIO_NUM_2
#define LCD_SPI_CLK gpio_num_t::GPIO_NUM_18
#define LCD_SPI_D gpio_num_t::GPIO_NUM_23
#define LCD_HEIGHT 240
#define LCD_WIDTH 240
#define LCD_SPI_HOST SPI2_HOST
#define LCD_PIXEL_CLOCK 60
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

// look:
// https://github.com/espressif/esp-bsp/blob/f7fd0914232306b4494442014679202d9e938d39/components/esp_lvgl_port/examples/touchscreen/main/main.c#L30
#define LCD_BIT_PER_PIXEL 16

#define LCD_TAG "LCD_INIT"

#define SPI_QUEUE_MAX_SIZE 7

esp_lcd_panel_io_handle_t lcdPanelIO_handle;
esp_lcd_panel_handle_t lcdPanel_handle;

lv_display_t *lvglDisplay = NULL;

void example_lvgl_demo_ui(lv_display_t *disp) {
  lv_obj_t *scr = lv_disp_get_scr_act(disp);
  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_long_mode(label,
                         LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
  lv_label_set_text(label, "Hello Espressif, Hello LVGL.");
  /* Size of the screen (if you use rotation 90 or 270, please set
   * disp->driver->ver_res) */
  lv_obj_set_width(label, disp->hor_res);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
}

void driverInitialization() {
  spi_device_handle_t lcdSpi_handle;

  spi_bus_config_t lcdSpi_busConfig = {
      .mosi_io_num = LCD_SPI_D,
      .miso_io_num = -1,
      .sclk_io_num = LCD_SPI_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };

  spi_device_interface_config_t lcdSpi_devInterfaceConfig = {
      .mode = 0,
      .clock_speed_hz = 10 * 1000 * 1000, // Clock out at 10 MHz
      .spics_io_num = LCD_SPI_CS,
      .queue_size = SPI_QUEUE_MAX_SIZE,
  };

  esp_err_t spiInitReturn =
      spi_bus_initialize(SPI2_HOST, &lcdSpi_busConfig, SPI_DMA_CH_AUTO);

  ESP_ERROR_CHECK(spiInitReturn);
  ESP_LOGI("INFO", "SPI Initialization Ret Val: %i", spiInitReturn);
}

void lcdPanelDriverInitialization() {
  /*
   * IO Initialization
   * */
  ESP_LOGI(LCD_TAG, "init IO SPI BUS");

  spi_bus_config_t lcdSpi_busConfig = {.mosi_io_num = LCD_SPI_D,
                                       .miso_io_num = -1,
                                       .sclk_io_num = LCD_SPI_CLK,
                                       .quadwp_io_num = -1,
                                       .quadhd_io_num = -1,
                                       .max_transfer_sz =
                                           LCD_HEIGHT * 80 * sizeof(uint16_t)};

  esp_lcd_panel_io_spi_config_t lcdPanel_spiConfig = {
      .cs_gpio_num = -1, // LCD_SPI_CS,// its not connected actually
      .dc_gpio_num = LCD_SPI_DC,
      .spi_mode = 0,
      .pclk_hz = LCD_PIXEL_CLOCK,
      .trans_queue_depth = 8,
      .lcd_cmd_bits = LCD_CMD_BITS,
      .lcd_param_bits = LCD_PARAM_BITS,
  };

  esp_err_t initRetVal =
      spi_bus_initialize(LCD_SPI_HOST, &lcdSpi_busConfig, SPI_DMA_CH_AUTO);

  ESP_ERROR_CHECK(initRetVal);

  if (initRetVal == ESP_OK) {
    ESP_LOGI(LCD_TAG, "Lcd Driver Initialization Success");
  }

  initRetVal =
      esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST,
                               &lcdPanel_spiConfig, &lcdPanelIO_handle);

  ESP_ERROR_CHECK(initRetVal);

  ESP_LOGI(LCD_TAG, "init LCD Panel");

  /*
   * LCD Panel Initialization
   * */

  esp_lcd_panel_dev_config_t lcdPanel_config = {
      .reset_gpio_num = LCD_SPI_RST,
      .rgb_ele_order = lcd_rgb_element_order_t::LCD_RGB_ELEMENT_ORDER_BGR,
      .bits_per_pixel = LCD_BIT_PER_PIXEL};

  initRetVal = esp_lcd_new_panel_st7789(lcdPanelIO_handle, &lcdPanel_config,
                                        &lcdPanel_handle);

  ESP_ERROR_CHECK(initRetVal);

  esp_lcd_panel_reset(lcdPanel_handle);
  esp_lcd_panel_init(lcdPanel_handle);
}

void lvglAppInitialization() {
  ESP_LOGI("LVGL_APP_INIT", "Init LVGL");

  // TODO: try to understanding this config
  const lvgl_port_cfg_t lvglConfig = {
      .task_priority = 4,  /* LVGL task priority */
      .task_stack = 4096,  /* LVGL task stack size */
      .task_affinity = -1, /* LVGL task pinned to core (-1 is no affinity) */
      .task_max_sleep_ms = 500, /* Maximum sleep in LVGL task */
      .timer_period_ms = 5      /* LVGL timer tick period in ms */
  };

  ESP_ERROR_CHECK(lvgl_port_init(&lvglConfig));

  const lvgl_port_display_cfg_t lvglDisplay_config = {
      .io_handle = lcdPanelIO_handle,
      .panel_handle = lcdPanel_handle,
      .buffer_size = LCD_HEIGHT * 50,
      .double_buffer = true,
      .hres = LCD_HEIGHT,
      .vres = LCD_WIDTH,
      .monochrome = false,

      .rotation =
          {
              .swap_xy = false,
              .mirror_x = true,
              .mirror_y = true,
          },

      .color_format = LV_COLOR_FORMAT_RGB565,
      .flags =
          {
              .buff_dma = true,
              .swap_bytes = true,
          },
  };

  lvglDisplay = lvgl_port_add_disp(&lvglDisplay_config);
}

void lvglMainGui() {

  lvgl_port_lock(0);

  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *label = lv_label_create(screen);
  lv_obj_set_width(label, LCD_WIDTH);
  /*lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);*/

  lv_label_set_text(label, LV_SYMBOL_BELL
                    " Hello world Espressif and LVGL " LV_SYMBOL_BELL
                    "\n " LV_SYMBOL_WARNING
                    " For simplier initialization, use BSP " LV_SYMBOL_WARNING);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);

  lvgl_port_unlock();
}

void demoLvglGuiST7789() {
  lcdPanelDriverInitialization();
  lvglAppInitialization();

  /*lv_timer_handler();*/
  lvglMainGui();
}

#endif
