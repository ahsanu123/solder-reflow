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
#include <cmath>
#include <cstdint>
#include <esp_adc/adc_cali.h>
#include <sys/types.h>

#define ESP32_ADC_MAX_VOLTAGE 3.3
#define ESP32_ADC_MAX_BITWIDTH 4095.0
// there is a document for this, read more if curious

// rtd use Callendar-Van Dusen equation.
// R(T) = R0(1 + A × T + B × T2 – 100 × C × T3 + C × T4)
// references:
// https://www.mouser.com/pdfDocs/AN7186.pdf?srsltid=AfmBOopeBfHLf53xncZfWz4oKvHtpG818oGd7nUIX1V4p0UtETFTVqrB
#define RTD_CONST_A 3.90830e-3
#define RTD_CONST_B -5.775e-7
#define RTD_CONST_C 4.18301e-10
#define RTD_CONST_R0 100.0 // pt100 0 degre resistance

// RTD_CONST_C its conditional value based on current position, look
// references for details

adc_channel_t           channel[3] = {ADC_CHANNEL_7, ADC_CHANNEL_6, ADC_CHANNEL_5};

TaskHandle_t            adcTaskHandle;
QueueHandle_t           queueHandle;
adc_continuous_handle_t globalAdcHandle = NULL;

#define ADC_READ_LEN 256
#define QUEUE_LENGTH 256
#define MS_DELAY(TIME) TIME / portTICK_PERIOD_MS

float calculateTemperatureInC(uint16_t rawData) {
  float RTD_voltage           = rawData * (ESP32_ADC_MAX_VOLTAGE / ESP32_ADC_MAX_BITWIDTH);
  float RTD_resistance        = RTD_voltage / 1e-3;

  float underRoot             = (pow(RTD_CONST_A, 2)) - 4 * RTD_CONST_B * (1 - RTD_resistance / RTD_CONST_R0);
  float squareRootResult      = sqrt(underRoot);

  float RTD_temperatureResult = (-1 * RTD_CONST_A + squareRootResult) / (2 * RTD_CONST_B);

  return RTD_resistance;
}

static bool
adcConversionCompleteCallback(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data) {
  BaseType_t yield = pdFALSE;
  vTaskNotifyGiveFromISR(adcTaskHandle, &yield);
  return (yield == pdTRUE);
}

void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle) {

  adc_continuous_handle_t     handle     = NULL;
  adc_continuous_handle_cfg_t adc_config = {
    .max_store_buf_size = 1024,
    .conv_frame_size    = ADC_READ_LEN,
  };

  ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

  adc_continuous_config_t dig_cfg = {
    .sample_freq_hz = 1000 * 1000,
    .conv_mode      = adc_digi_convert_mode_t::ADC_CONV_SINGLE_UNIT_1,
    .format         = adc_digi_output_format_t::ADC_DIGI_OUTPUT_FORMAT_TYPE1,
  };

  adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};

  dig_cfg.pattern_num                                         = channel_num;

  for (int i = 0; i < channel_num; i++) {
    adc_pattern[i].atten     = ADC_ATTEN_DB_0;
    adc_pattern[i].channel   = channel[i] & 0x7;
    adc_pattern[i].unit      = adc_unit_t::ADC_UNIT_1;
    adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
  }

  dig_cfg.adc_pattern = adc_pattern;
  ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

  *out_handle = handle;
}

void adcProcessor(void *parameter) {
  uint32_t          retNum;
  uint8_t           result[ADC_READ_LEN] = {0};
  uint16_t          avgResult            = 0;
  uint32_t          bufResult            = 0;
  uint16_t          count                = 0;
  esp_err_t         ret;
  adc_cali_handle_t adcCaliHandle;

  continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &globalAdcHandle);

  adc_continuous_evt_cbs_t cbs = {
    .on_conv_done = adcConversionCompleteCallback,
  };

  ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(globalAdcHandle, &cbs, NULL));
  ESP_ERROR_CHECK(adc_continuous_start(globalAdcHandle));

  while (true) {
    ulTaskNotifyTake(pdTRUE, MS_DELAY(1));
    ret = adc_continuous_read(globalAdcHandle, result, ADC_READ_LEN, &retNum, 0);
    if (ret == ESP_OK) {
      for (int i = 0; i < retNum; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *outputPtr = (adc_digi_output_data_t *)&result[i];

        bufResult                         = outputPtr->type1.data;
        uint32_t data                     = outputPtr->type1.data;
        uint32_t chan_num                 = outputPtr->type1.channel;

        /*if (outputPtr->type1.channel == 7) {*/
        /*ESP_LOGI("info", "Channel: %" PRIu32 ", Value: %" PRIu32, chan_num, data);*/
        esp_log_write(ESP_LOG_INFO, "", "%" PRIu32 "\n", bufResult);
        /*}*/
      }
      xQueueSendToBack(queueHandle, &avgResult, pdMS_TO_TICKS(20));
      avgResult = 0;
      count     = 0;
    }
    vTaskDelay(2);
  }
}

void adcMessageFlusher(void *parameter) {
  BaseType_t xStatus;
  uint16_t   receivedData     = 0;
  float      calculatedResult = 0.0;
  /*demoLvglGuiST7789();*/
  while (true) {

    if (uxQueueMessagesWaiting(queueHandle) != QUEUE_LENGTH) {
      /*ESP_LOGI("INFO", "...");*/
    }

    xStatus = xQueueReceive(queueHandle, &receivedData, 0);

    if (xStatus == pdPASS) {
      /*esp_log_write(ESP_LOG_INFO, "", "%f,\n",*/
      /*              calculateTemperatureInC(receivedData));*/
      ESP_LOGI("", "%" PRIu16, receivedData);
      /*esp_log_write(ESP_LOG_INFO, "", "%" PRIu16 "\n", receivedData);*/

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
    xTaskCreate(adcMessageFlusher, "adcMessageFlusher", 4096 * 2, NULL, 1, NULL);
  }
}

#endif
