#include "./MainTempSensor.h"
#include "esp_attr.h"
#include "esp_err.h"
#include <esp_adc/adc_continuous.h>

#ifndef INCLUDE_SRC_PT100_H_
#define INCLUDE_SRC_PT100_H_

// temperature increase for each resistance unit
#define TEMP_CONSTANT 0.3619
#define TEMP_M 2.6773
#define TEMP_C -267.73
#define TEMP_CURRENT_CONSTANT 0.00113636
#define TEMP_GAIN_CONSTANT 11.63829

adc_continuous_handle_cfg_t adcConfig = {.max_store_buf_size = 1024,
                                         .conv_frame_size = 100};

#ifdef __cplusplus
extern "C" {
#endif

class PT100 : public TempDevice {
  adc_continuous_handle_t handle;

public:
  float GetTemp() override { return 1.9; }
  uint16_t GetRawAdc() override { return 21; }

  bool InitDevice() override {
    auto handleResult = adc_continuous_new_handle(&adcConfig, &handle);
    ESP_ERROR_CHECK(handleResult);

    auto startResult = adc_continuous_start(handle);
    ESP_ERROR_CHECK(startResult);

    return startResult == ESP_OK ? true : false;
  }
};

/*class PT100 {*/
/*  float temp;*/
/*  adc_config_t adcConfig;*/
/**/
/*public:*/
/*  float process();*/
/*  std::string debug();*/
/**/
/*  PT100();*/
/*};*/

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_SRC_PT100_H_
