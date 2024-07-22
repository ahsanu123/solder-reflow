#include "Device.h"

#ifndef INCLUDE_ADC_DEVICE
#define INCLUDE_ADC_DEVICE

class ADCDevice : public Device {
public:
  esp_err_t Begin() override {}
};

#endif
