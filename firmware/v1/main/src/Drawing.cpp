
#include "Drawing.h"
#include "hw_timer.h"
#include "util.h"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>

#ifndef INCLUDE_SRC_DRAWING_CPP_
#define INCLUDE_SRC_DRAWING_CPP_

Drawing::Drawing(DisplaySSD1306_128x32_I2C &display, PT100 &pt100) {

  float initialTemp = (this->_pt100)->process();
  uint32_t initialTime = hw_timer_get_count_data();

  this->_display = &display;
  this->_pt100 = &pt100;

  for (int i = 0; i < sizeof(this->_oldTemp) / sizeof(float); i++) {
    this->_oldTemp[i] = initialTemp;
  }

  for (int i = 0; i < sizeof(this->_oldTimer) / sizeof(uint32_t); i++) {
    this->_oldTimer[i] = initialTime;
  }

  for (int i = 0; i < sizeof(this->_x) / sizeof(int); i++) {
    this->_oldTimer[i] = this->_display->width() / 6;
  }
}

// BUG: still messy here, clean it!!
void Drawing::plotTemp() {

  this->_display->clear();

  float newTemp[6];
  uint32_t newTimer[6];

  for (int i = 0; i < sizeof(this->_oldTemp) / sizeof(float); i++) {
    newTemp[i] = this->_oldTemp[i];
  }

  for (int i = 0; i < sizeof(this->_oldTemp) / sizeof(uint32_t); i++) {
    newTimer[i] = this->_oldTimer[i];
  }

  newTemp[5] = this->_pt100->process();
  newTimer[5] = hw_timer_get_count_data();
  this->_display->printFixedPgm(0, 0, std::to_string(newTemp[5]).c_str());

  // BUG: this drawwingline is not normalized wiill chopped
  // when running, fixthis
  for (int i = 0; i < 6 / 2; i++) {
    this->_display->drawLine(this->_x[i], newTemp[i], this->_x[i + 1],
                             newTemp[i + 1]);
  }

  for (int i = 0; i < 6 - 1; i++) {
    this->_oldTemp[i] = this->_oldTemp[i + 1];
    this->_oldTimer[i] = this->_oldTimer[i + 1];
  }
  this->_oldTemp[5] = newTemp[5];
  this->_oldTimer[5] = newTimer[5];
}

void Drawing::drawProfile(float temp[]) {

  float normalizedTemp[6];
  float normalizedTime[6] = {0, 10, 20, 30, 50, 60};
  char buff[10];

  for (int i = 0; i < 6; i++) {
    normalizedTemp[i] = map(temp[i], 0, 270, 0, this->_display->height());

    normalizedTime[i] =
        map(normalizedTime[i], 0, 60, 0, this->_display->width());
  }
  for (int i = 0; i < 6 - 1; i++) {

    this->_display->drawLine(normalizedTime[i], normalizedTemp[i],
                             normalizedTime[i + 1], normalizedTemp[i + 1]);

    sprintf(buff, "%d", (int)floor(temp[i]));
    this->_display->printFixed(normalizedTime[i], normalizedTemp[i], buff);
  }
}

#endif // INCLUDE_SRC_DRAWING_CPP_
