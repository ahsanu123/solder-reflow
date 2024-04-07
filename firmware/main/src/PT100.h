#include "driver/adc.h"
#include <string>

#ifndef INCLUDE_SRC_PT100_H_
#define INCLUDE_SRC_PT100_H_

// temperature increase for each resistance unit
#define TEMP_CONSTANT 0.3619
#define TEMP_M 2.6773
#define TEMP_C -267.73
#define TEMP_CURRENT_CONSTANT 1.13636
#define TEMP_GAIN_CONSTANT 11.63829

#ifdef __cplusplus
extern "C" {
#endif

class PT100 {
  float temp;
  adc_config_t adcConfig;

public:
  float process();
  std::string debug();

  PT100();
};

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_SRC_PT100_H_
