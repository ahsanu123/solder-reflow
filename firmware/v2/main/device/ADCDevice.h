
#ifndef INCLUDE_ADC_DEVICE
#define INCLUDE_ADC_DEVICE

#include "Device.h"
#include "esp_adc/adc_continuous.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "soc/soc_caps.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

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

#define ESP32_ADC_MAX_VOLTAGE 3.3
#define ESP32_ADC_MAX_BITWIDTH 4096.0
#define RTD_RESISTANCE_CALIBRATION_CONSTANT 4.5
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

#define ADC_BUFFER_LEN 256
#define ADC_UNIT_STR(unit) #unit
#define NUM_OF_ADC 1

#define TEMP_SENSOR_PIN ADC_CHANNEL_6

typedef struct AdcStruct_t {
  uint16_t      rawValue;
  float         normalizeValue;
  adc_channel_t channelNum;
} AdcStruct;

std::array<AdcStruct, NUM_OF_ADC> defaultAdcDevice = {
  AdcStruct{.rawValue = 0, .normalizeValue = 0, .channelNum = TEMP_SENSOR_PIN},
  /*AdcStruct{.rawValue = 0, .normalizeValue = 0, .channelNum = adc_channel_t::ADC_CHANNEL_7}*/
};

typedef void (*InitAdcDevicesFunc)(adc_continuous_handle_t &handle);
typedef adc_continuous_handle_t (*InitAdcHandle)(std::array<AdcStruct, NUM_OF_ADC> &adcs, uint32_t sampleFreqInHz);

float normalizeAdcFuntion(uint32_t rawValue) { return (rawValue / 1024) * 3.3; }

adc_continuous_handle_t
defaultInitAdcHandle(std::array<AdcStruct, NUM_OF_ADC> &adcs, uint32_t sampleFreqInHz = 500000) {

  // Look to documentation for step to use cotinuous adc in esp32
  // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/adc_continuous.html

  uint32_t                    numOfChannel = adcs.size();
  adc_continuous_handle_t     handle       = NULL;
  adc_continuous_handle_cfg_t handleConfig = {.max_store_buf_size = 1024 * 4, .conv_frame_size = 256};
  auto                        handleRet    = adc_continuous_new_handle(&handleConfig, &handle);
  ESP_ERROR_CHECK(handleRet);

  adc_digi_pattern_config_t adcPattern[SOC_ADC_PATT_LEN_MAX] = {0};

  for (int i = 0; i < numOfChannel; i++) {
    adcPattern[i].atten     = ADC_ATTEN_DB_0; // ADC_ATTEN_DB_0;
    adcPattern[i].channel   = adcs.at(i).channelNum & 0x7;
    adcPattern[i].unit      = ADC_UNIT_1;
    adcPattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
  }

  adc_continuous_config_t adcContinuousConfig = {
    .pattern_num    = numOfChannel,
    .adc_pattern    = adcPattern,
    .sample_freq_hz = sampleFreqInHz,
    .conv_mode      = ADC_CONV_SINGLE_UNIT_1,
    .format         = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
  };

  esp_err_t initRet = adc_continuous_config(handle, &adcContinuousConfig);
  ESP_ERROR_CHECK(initRet);

  return handle;
}

float raw2temp(uint16_t rawData) {
  float RTD_voltage =
    ((rawData / RTD_RESISTANCE_CALIBRATION_CONSTANT) / ESP32_ADC_MAX_BITWIDTH) * ESP32_ADC_MAX_VOLTAGE;
  float RTD_resistance        = RTD_voltage / 1e-3;

  float underRoot             = (pow(RTD_CONST_A, 2)) - ((4 * RTD_CONST_B) * (1 - (RTD_resistance / RTD_CONST_R0)));
  float squareRootResult      = sqrt(underRoot);

  float RTD_temperatureResult = ((-1 * RTD_CONST_A) + squareRootResult) / (2 * RTD_CONST_B);

  return RTD_temperatureResult;
}

float calculateTemperatureInC(uint16_t rawData) {
  float RTD_voltage           = (rawData / ESP32_ADC_MAX_BITWIDTH) * ESP32_ADC_MAX_VOLTAGE;
  float RTD_resistance        = RTD_voltage / 1e-3;

  float underRoot             = (pow(RTD_CONST_A, 2)) - ((4 * RTD_CONST_B) * (1 - (RTD_resistance / RTD_CONST_R0)));
  float squareRootResult      = sqrt(underRoot);

  float RTD_temperatureResult = ((-1 * RTD_CONST_A) + squareRootResult) / (2 * RTD_CONST_B);

  return RTD_temperatureResult;
}

void defaultInitAdcDevices(adc_continuous_handle_t &handle) {
  esp_err_t retval = adc_continuous_start(handle);
  ESP_ERROR_CHECK(retval);
}

class NewAdcDevice {
private:
  std::array<AdcStruct, NUM_OF_ADC> adcDev;
  adc_continuous_handle_t           handle;
  uint8_t                           buffer[ADC_BUFFER_LEN];
  bool                              isShowlog;

public:
  NewAdcDevice(
    std::array<AdcStruct, NUM_OF_ADC> adcDevices            = defaultAdcDevice,
    InitAdcHandle                     initAdcHandleFunction = defaultInitAdcHandle
  ) {
    this->adcDev    = adcDevices;
    this->handle    = initAdcHandleFunction(this->adcDev, 20000);
    this->isShowlog = false;
  }

  uint16_t GetRawValue(int index) {
    /*if(index<ADC_BUFFER_LEN || index>ADC_BUFFER_LEN) return;*/
    return this->adcDev.at(0).rawValue;
  }
  void Init(InitAdcDevicesFunc initAdcDevices = defaultInitAdcDevices) { initAdcDevices(this->handle); }

  void showLog(bool show) { this->isShowlog = show; }

  void Scan() {
    uint32_t  outLen;
    int       index  = 0;
    char      unit[] = ADC_UNIT_STR(ADC_UNIT_1);

    esp_err_t ret    = adc_continuous_read(this->handle, this->buffer, ADC_BUFFER_LEN, &outLen, 0);

    if (ret == ESP_OK) {
      for (int i = 0; i < outLen; i += SOC_ADC_DIGI_RESULT_BYTES) {

        // this really weird type casting from documentation
        auto     dataPointer        = (adc_digi_output_data_t *)&this->buffer[i];
        uint32_t chanNum            = dataPointer->type1.channel;
        this->adcDev.at(0).rawValue = dataPointer->type1.data;

        // =================================
        /*if (this->isShowlog) {*/
        /*  if (index < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1)) {*/
        /*    ESP_LOGI("Info", "Unit: %s, Channel: %" PRIu32 ", Value: %"
         * PRIx32,*/
        /*             unit, chanNum, this->adcDev.at(index).rawValue);*/
        /*  } else {*/
        /*    ESP_LOGW("ERROR", "Invalid data [%s_%" PRIu32 "_%" PRIx32 "]",
         * unit,*/
        /*             chanNum, this->adcDev.at(index).rawValue);*/
        /*  }*/
        /*}*/
        // =================================
        index++;
      }
    }
  }
};

class ADCDevice : public Device {

#if CONFIG_IDF_TARGET_ESP32
  adc_channel_t channel[2] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
#else
  adc_channel_t channel[2] = {ADC_CHANNEL_2, ADC_CHANNEL_3};
#endif

  adc_continuous_handle_t handle = NULL;
  int                     channelNum;
  uint8_t                 buffer[ADC_BUFFER_LEN];

public:
  ~ADCDevice() {
    adc_continuous_stop(this->handle);
    adc_continuous_deinit(this->handle);
  }

  esp_err_t Init() override {
    adc_continuous_handle_t     handle       = NULL;
    adc_continuous_handle_cfg_t handleConfig = {.max_store_buf_size = 1024, .conv_frame_size = 256};

    auto                        handleRet    = adc_continuous_new_handle(&handleConfig, &handle);
    ESP_ERROR_CHECK(handleRet);

    uint8_t channelNum                    = sizeof(this->channel) / sizeof(adc_channel_t);
    this->channelNum                      = channelNum;

    adc_continuous_config_t adcDigiConfig = {
      .pattern_num    = channelNum,
      .sample_freq_hz = 1000 * 1000,
      .conv_mode      = ADC_CONV_SINGLE_UNIT_1,
      .format         = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };

    adc_digi_pattern_config_t adcPattern[SOC_ADC_PATT_LEN_MAX] = {0};

    for (int i = 0; i < channelNum; i++) {
      adcPattern[i].atten     = ADC_ATTEN_DB_0;
      adcPattern[i].channel   = this->channel[i] & 0x7;
      adcPattern[i].unit      = ADC_UNIT_1;
      adcPattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }

    adcDigiConfig.adc_pattern = adcPattern;

    esp_err_t initRet         = adc_continuous_config(handle, &adcDigiConfig);
    ESP_ERROR_CHECK(initRet);

    this->handle = handle;

    return ESP_OK;
  }

  // NOTE: Make sure to add IRAM_ATTR attribute
  void RegisterCallback(adc_continuous_callback_t *callback) override {
    adc_continuous_evt_cbs_t callbackConfig = {.on_conv_done = *callback};
    auto registerCallbackRet = adc_continuous_register_event_callbacks(this->handle, &callbackConfig, NULL);
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
    uint32_t  outLen;
    char      unit[] = ADC_UNIT_STR(ADC_UNIT_1);

    esp_err_t ret    = adc_continuous_read(this->handle, this->buffer, ADC_BUFFER_LEN, &outLen, 0);

    if (ret == ESP_OK) {

      for (int i = 0; i < outLen; i += SOC_ADC_DIGI_RESULT_BYTES) {

        auto     dataPointer = (adc_digi_output_data_t *)&this->buffer[i];

        uint32_t chanNum     = dataPointer->type1.channel;
        uint32_t rawData     = dataPointer->type1.data;

        ESP_LOGI("INFO", "outlen: %" PRIu32, outLen);
        ESP_LOGI("INFO", "i: %i", i);
        ESP_LOGI("INFO", "channel: %" PRIu32, chanNum);
        ESP_LOGI("INFO", "SOC_ADC_DIGI_RESULT_BYTES: %i", SOC_ADC_DIGI_RESULT_BYTES);

        if (channelNum < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1)) {
          ESP_LOGI("Info", "Unit: %s, Channel: %" PRIu32 ", Value: %" PRIx32, unit, chanNum, rawData);
        } else {
          ESP_LOGW("ERROR", "Invalid data [%s_%" PRIu32 "_%" PRIx32 "]", unit, chanNum, rawData);
        }
      }
    }
    return 2;
  }
};

#endif
