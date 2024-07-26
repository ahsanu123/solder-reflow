#include "Device.h"
#include "esp_adc/adc_continuous.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "soc/soc_caps.h"
#include <cstddef>
#include <cstdint>

#ifndef INCLUDE_ADC_DEVICE
#define INCLUDE_ADC_DEVICE

// clang-format off

/*
 * =============================================
 * My conclusion to setup adc in esp32 with idf 
 * =============================================
 *
 * - first make adc handle with handleConfig
 * - then init your handle with adc_continuous_config() based on your handleConfig
 * - in handleConfig you need to spesify what channel you want use
 * - if you want you can add callback with adc_continuous_register_event_callbacks()
 * - finally you can start adc driver with adc_continuous_start() based on your handle
 *
 * */

// clang-format on

#define ADC_BUFFER_LEN 256
#define ADC_UNIT_STR(unit) #unit

class ADCDevice : public Device {

#if CONFIG_IDF_TARGET_ESP32
  adc_channel_t channel[2] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
#else
  adc_channel_t channel[2] = {ADC_CHANNEL_2, ADC_CHANNEL_3};
#endif

  adc_continuous_handle_t handle = NULL;
  int channelNum;
  uint8_t buffer[ADC_BUFFER_LEN];

public:
  ~ADCDevice() {
    adc_continuous_stop(this->handle);
    adc_continuous_deinit(this->handle);
  }

  esp_err_t Init() override {
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t handleConfig = {.max_store_buf_size = 1024,
                                                .conv_frame_size = 256};

    auto handleRet = adc_continuous_new_handle(&handleConfig, &handle);
    ESP_ERROR_CHECK(handleRet);

    uint8_t channelNum = sizeof(this->channel) / sizeof(adc_channel_t);
    this->channelNum = channelNum;

    adc_continuous_config_t adcDigiConfig = {
        .pattern_num = channelNum,
        .sample_freq_hz = 20 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };

    adc_digi_pattern_config_t adcPattern[SOC_ADC_PATT_LEN_MAX] = {0};

    for (int i = 0; i < channelNum; i++) {
      adcPattern[i].atten = ADC_ATTEN_DB_0;
      adcPattern[i].channel = this->channel[i] & 0x7;
      adcPattern[i].unit = ADC_UNIT_1;
      adcPattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }

    adcDigiConfig.adc_pattern = adcPattern;

    esp_err_t initRet = adc_continuous_config(handle, &adcDigiConfig);
    ESP_ERROR_CHECK(initRet);

    this->handle = handle;

    return ESP_OK;
  }

  // NOTE: Make sure to add IRAM_ATTR attribute
  void RegisterCallback(adc_continuous_callback_t *callback) override {
    adc_continuous_evt_cbs_t callbackConfig = {.on_conv_done = *callback};
    auto registerCallbackRet = adc_continuous_register_event_callbacks(
        this->handle, &callbackConfig, NULL);
    ESP_ERROR_CHECK(registerCallbackRet);
  }

  esp_err_t Begin() override {
    esp_err_t retval = adc_continuous_start(this->handle);
    ESP_ERROR_CHECK(retval);
    return retval;
  }

  // TODO: make function to get raw value based on channel

  uint32_t GetRawValue(int channelNum) override {
    /*if (channelNum > this->channelNum) {*/
    /*  return 0;*/
    /*}*/
    uint32_t outLen;
    char unit[] = ADC_UNIT_STR(ADC_UNIT_1);

    esp_err_t ret = adc_continuous_read(this->handle, this->buffer,
                                        ADC_BUFFER_LEN, &outLen, 0);

    if (ret == ESP_OK) {

      for (int i = 0; i < outLen; i += SOC_ADC_DIGI_RESULT_BYTES) {

        auto dataPointer = (adc_digi_output_data_t *)&this->buffer[i];

        uint32_t chanNum = dataPointer->type1.channel;
        uint32_t rawData = dataPointer->type1.data;

        if (channelNum < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1)) {
          ESP_LOGI("Info", "Unit: %s, Channel: %" PRIu32 ", Value: %" PRIx32,
                   unit, chanNum, rawData);
        } else {
          ESP_LOGW("ERROR", "Invalid data [%s_%" PRIu32 "_%" PRIx32 "]", unit,
                   chanNum, rawData);
        }
      }
    }
    return 2;
  }
};

#endif
