#include "PT100.h"

#include <cstdint>

#include "adc.h"

#ifndef INCLUDE_SRC_PT100_CPP_
#define INCLUDE_SRC_PT100_CPP_

PT100::PT100(adc_config_t &config) {
  this->adcConfig = config;
  adc_init(&this->adcConfig);
}

float PT100::process() {

  /**
   * Manual Measurement
   * Current I from opamp is 1.13mA due lack of
   * 2.5k resistor (use 2.2k)
   *
   * this Measurement use I=1mA
   *| temp |  R     |   V     | Gain | Vout  |
   *| 0    | 100    | 0.1     | 11.6 | 1.16  |
   *| 100  | 138.5  | 0.1385  | 11.6 | 1.606 |
   *| 150  | 157.33 | 0.15733 | 11.6 | 1.825 |
   *| 200  | 175.86 | 0.17586 | 11.6 | 2.039 |
   *| 250  | 194.1  | 0.1941  | 11.6 | 2.251 |
   *| 300  | 212.05 | 0.21204 | 11.6 | 2.459 |
   * */

  uint16_t adc_raw;
  adc_read(&adc_raw);

  float voltage = (adc_raw / 1023.0) * 3.3;
  float temp =
      ((voltage / TEMP_GAIN_CONSTANT) / TEMP_CURRENT_CONSTANT) * TEMP_M +
      TEMP_C;

  return temp;
}

#endif // INCLUDE_SRC_PT100_CPP_
