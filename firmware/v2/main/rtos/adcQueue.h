// based on esp32 example
// https://github.com/espressif/esp-idf/blob/v5.3/examples/peripherals/adc/continuous_read/main/continuous_read_main.c
#ifndef ADC_QUEUE
#define ADC_QUEUE

#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/adc_types.h"
#include "portmacro.h"
#include "taskNotification.h"
#include <cstdint>
#include <sys/types.h>

#define ADC_READ_LEN 256
#define QUEUE_LENGTH 256
#define MS_DELAY(TIME) TIME / portTICK_PERIOD_MS

adc_channel_t channel[2] = {ADC_CHANNEL_7 /*, ADC_CHANNEL_6*/};

TaskHandle_t adcTaskHandle;
QueueHandle_t queueHandle;
adc_continuous_handle_t globalAdcHandle = NULL;

static bool
adcConversionCompleteCallback(adc_continuous_handle_t handle,
                              const adc_continuous_evt_data_t *edata,
                              void *user_data) {
  BaseType_t yield = pdFALSE;
  vTaskNotifyGiveFromISR(adcTaskHandle, &yield);

  return (yield == pdTRUE);
}

void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num,
                         adc_continuous_handle_t *out_handle) {

  adc_continuous_handle_t handle = NULL;

  adc_continuous_handle_cfg_t adc_config = {
      .max_store_buf_size = 1024,
      .conv_frame_size = ADC_READ_LEN,
  };

  ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

  adc_continuous_config_t dig_cfg = {
      .sample_freq_hz = 20 * 1000,
      .conv_mode = adc_digi_convert_mode_t::ADC_CONV_SINGLE_UNIT_1,
      .format = adc_digi_output_format_t::ADC_DIGI_OUTPUT_FORMAT_TYPE1,
  };

  adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};

  dig_cfg.pattern_num = channel_num;

  for (int i = 0; i < channel_num; i++) {
    adc_pattern[i].atten = adc_digi_convert_mode_t::ADC_CONV_SINGLE_UNIT_1;
    adc_pattern[i].channel = channel[i] & 0x7;
    adc_pattern[i].unit = adc_unit_t::ADC_UNIT_1;
    adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
  }

  dig_cfg.adc_pattern = adc_pattern;
  ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

  *out_handle = handle;
}

void adcProcessor(void *parameter) {
  uint32_t retNum;
  uint8_t result[ADC_READ_LEN] = {0};
  uint16_t avgResult = 0;
  uint16_t bufResult = 0;
  esp_err_t ret;

  continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t),
                      &globalAdcHandle);

  adc_continuous_evt_cbs_t cbs = {
      .on_conv_done = adcConversionCompleteCallback,
  };

  ESP_ERROR_CHECK(
      adc_continuous_register_event_callbacks(globalAdcHandle, &cbs, NULL));
  ESP_ERROR_CHECK(adc_continuous_start(globalAdcHandle));

  while (true) {
    ulTaskNotifyTake(pdTRUE, MS_DELAY(1));
    ret =
        adc_continuous_read(globalAdcHandle, result, ADC_READ_LEN, &retNum, 0);
    if (ret == ESP_OK) {
      for (int i = 0; i < retNum; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (adc_digi_output_data_t *)&result[i];

        bufResult = p->type1.data;
        avgResult += bufResult;

        /*esp_log_write(ESP_LOG_INFO, "", "%" PRIu16 "\n", bufResult);*/
        /*if (i == 1 || i == 10 || i == 100) {*/
        /*  ESP_LOGI("INFO", "instant: %" PRIu16, bufResult);*/
        /*}*/
      }
      xQueueSendToBack(queueHandle, &avgResult, pdMS_TO_TICKS(20));
      avgResult = 0;
    }
  }
}

void adcMessageFlusher(void *parameter) {
  BaseType_t xStatus;
  uint16_t receivedData = 0;
  float calculatedResult = 0.0;
  while (true) {

    if (uxQueueMessagesWaiting(queueHandle) != QUEUE_LENGTH) {
      /*ESP_LOGI("INFO", "...");*/
    }

    xStatus = xQueueReceive(queueHandle, &receivedData, 0);

    if (xStatus == pdPASS) {
      /*ESP_LOGI("", "%" PRIu16, receivedData);*/
      esp_log_write(ESP_LOG_INFO, "", "%" PRIu16 "\n", receivedData);

      /*esp_log_write(ESP_LOG_INFO, "", "%" PRIu16 ", %f\n", receivedData,*/
      /*              calculatedResult);*/
    }

    vTaskDelay(MS_DELAY(10));
  }
}

void demoAdcQueueData() {
  queueHandle = xQueueCreate(QUEUE_LENGTH, sizeof(uint16_t));

  if (queueHandle != NULL) {
    xTaskCreate(adcProcessor, "adcProcessor", 4096, NULL, 2, &adcTaskHandle);
    xTaskCreate(adcMessageFlusher, "adcMessageFlusher", 2048, NULL, 1, NULL);
  }
}

#endif
