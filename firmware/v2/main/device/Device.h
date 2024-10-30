#ifndef INCLUDE_DEVICE
#define INCLUDE_DEVICE

#include "esp_err.h"
#include <cstdint>
#include <esp_adc/adc_continuous.h>

class Device {
public:
  virtual esp_err_t Begin()                                               = 0;
  virtual esp_err_t Init()                                                = 0;
  virtual void      RegisterCallback(adc_continuous_callback_t *callback) = 0;
  virtual uint32_t  GetRawValue(int channelNum)                           = 0;
};

#endif
