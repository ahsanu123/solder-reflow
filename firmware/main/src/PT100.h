#include "driver/adc.h"
#include <string>

// temperature increase for each resistance unit
#define TEMP_CONSTANT 0.3619
#define TEMP_M 2.6773
#define TEMP_C -267.73
#define TEMP_CURRENT_CONSTANT 1.13636
#define TEMP_GAIN_CONSTANT 11.63829

class PT100 {
  float temp;
  adc_config_t adcConfig;

public:
  float process();
  std::string debug();

  PT100(adc_config_t &config);
};
