#include "PT100.h"
#include "driver/hw_timer.h"
#include "v2/lcd/ssd1306/lcd_ssd1306.h"
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

struct profile_t {
  float temp;
  float duration;
};

struct temp_profile_t {
  profile_t listProfile[6];
};

class Drawing {
  float _oldTemp[6];
  uint32_t _oldTimer[6];
  int _x[6];
  DisplaySSD1306_128x32_I2C *_display;
  PT100 *_pt100;

public:
  void plotTemp();
  void drawProfile(float temp[]);

  Drawing(DisplaySSD1306_128x32_I2C &display, PT100 &pt100);
};

#ifdef __cplusplus
}
#endif
