#include "device/ADCDevice.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"
#include "soc/gpio_num.h"
#include <stdio.h>
#include <string.h>

#define INPUT1 GPIO_NUM_27
#define INPUT2 GPIO_NUM_26
#define INPUT3 GPIO_NUM_25
#define INPUT4 GPIO_NUM_22
#define OUTPUT1 GPIO_NUM_4
#define OUTPUT2 GPIO_NUM_21

#define GPIO_INPUT_SELECTOR                                                    \
  ((1ULL << INPUT1) | (1ULL << INPUT2) | (1ULL << INPUT3) | (1ULL << INPUT4))
#define GPIO_OUTPUT_SELECTOR ((1ULL << OUTPUT1) | (1ULL << OUTPUT2))

#define EXAMPLE_ADC_UNIT ADC_UNIT_1
#define _EXAMPLE_ADC_UNIT_STR(unit) #unit
#define EXAMPLE_ADC_UNIT_STR(unit) _EXAMPLE_ADC_UNIT_STR(unit)
#define EXAMPLE_ADC_CONV_MODE ADC_CONV_SINGLE_UNIT_1
#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_0
#define EXAMPLE_ADC_BIT_WIDTH SOC_ADC_DIGI_MAX_BITWIDTH

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_ADC_OUTPUT_TYPE ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define EXAMPLE_ADC_GET_CHANNEL(p_data) ((p_data)->type1.channel)
#define EXAMPLE_ADC_GET_DATA(p_data) ((p_data)->type1.data)
#else
#define EXAMPLE_ADC_OUTPUT_TYPE ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define EXAMPLE_ADC_GET_CHANNEL(p_data) ((p_data)->type2.channel)
#define EXAMPLE_ADC_GET_DATA(p_data) ((p_data)->type2.data)
#endif

#define EXAMPLE_READ_LEN 256

#if CONFIG_IDF_TARGET_ESP32
static adc_channel_t channel[2] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
#else
static adc_channel_t channel[2] = {ADC_CHANNEL_2, ADC_CHANNEL_3};
#endif

static TaskHandle_t s_task_handle;
static const char *TAG = "EXAMPLE";

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle,
                                     const adc_continuous_evt_data_t *edata,
                                     void *user_data) {
  BaseType_t mustYield = pdFALSE;
  // Notify that ADC continuous driver has done enough number of conversions
  vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

  return (mustYield == pdTRUE);
}

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num,
                                adc_continuous_handle_t *out_handle) {
  adc_continuous_handle_t handle = NULL;

  adc_continuous_handle_cfg_t adc_config = {
      .max_store_buf_size = 1024,
      .conv_frame_size = EXAMPLE_READ_LEN,
  };
  ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

  adc_continuous_config_t dig_cfg = {
      .sample_freq_hz = 20 * 1000,
      .conv_mode = EXAMPLE_ADC_CONV_MODE,
      .format = EXAMPLE_ADC_OUTPUT_TYPE,
  };

  adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
  dig_cfg.pattern_num = channel_num;
  for (int i = 0; i < channel_num; i++) {
    adc_pattern[i].atten = EXAMPLE_ADC_ATTEN;
    adc_pattern[i].channel = channel[i] & 0x7;
    adc_pattern[i].unit = EXAMPLE_ADC_UNIT;
    adc_pattern[i].bit_width = EXAMPLE_ADC_BIT_WIDTH;

    ESP_LOGI(TAG, "adc_pattern[%d].atten is :%" PRIx8, i, adc_pattern[i].atten);
    ESP_LOGI(TAG, "adc_pattern[%d].channel is :%" PRIx8, i,
             adc_pattern[i].channel);
    ESP_LOGI(TAG, "adc_pattern[%d].unit is :%" PRIx8, i, adc_pattern[i].unit);
  }
  dig_cfg.adc_pattern = adc_pattern;
  ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

  *out_handle = handle;
}

#ifdef __cplusplus
extern "C" {
#endif

void app_main(void) {

  /*auto adcDev = new ADCDevice();*/
  /*adcDev->Init();*/
  /*adcDev->Begin();*/

  gpio_config_t ioConfig = {};

  ioConfig.mode = GPIO_MODE_INPUT;
  ioConfig.intr_type = GPIO_INTR_DISABLE;
  ioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
  ioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  ioConfig.pin_bit_mask = GPIO_INPUT_SELECTOR;
  gpio_config(&ioConfig);

  ioConfig.mode = GPIO_MODE_OUTPUT;
  ioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  ioConfig.pin_bit_mask = GPIO_OUTPUT_SELECTOR;
  gpio_config(&ioConfig);

  while (1) {
    if (!gpio_get_level(INPUT1)) {
      ESP_LOGI("INPUT", "Input 1 Pressed");
      gpio_set_level(OUTPUT1, 0);
    }

    if (!gpio_get_level(INPUT2)) {
      ESP_LOGI("INPUT", "Input 2 Pressed");
      gpio_set_level(OUTPUT2, 0);
    }

    if (!gpio_get_level(INPUT3)) {
      ESP_LOGI("INPUT", "Input 3 Pressed");
      gpio_set_level(OUTPUT1, 1);
    }

    if (!gpio_get_level(INPUT4)) {
      ESP_LOGI("INPUT", "Input 4 Pressed");
      gpio_set_level(OUTPUT2, 1);
    }

    vTaskDelay(10);
  }
}

#ifdef __cplusplus
}
#endif
