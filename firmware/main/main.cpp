#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canvas/fonts/fonts.h"
#include "driver/uart.h"
#include "gpio.h"
#include "i2c.h"
#include "lcd_hal/esp/esp8266_i2c.h"
#include "src/Drawing.h"
#include "src/Encoder.h"
#include "src/EspRotary.h"
#include "src/PidController.h"
#include "src/SimpleControl.h"
#include "v2/gui/menu.h"
#include "v2/lcd/ssd1306/lcd_ssd1306.h"

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
#define CLICKS_PER_STEP 4

enum Menu {
  MENU_SELECT = -1,
  MENU_START1,
  MENU_START2,
  MENU_START3,
  MENU_STOP,
  MENU_MONITOR,
  MENU_SETPOINT,
  MENU_ABOUT
};

// BUG: fix this
float dummyfloat;

profile_t profil1[6] = {{0, 0},   {10, 20}, {20, 30},
                        {30, 30}, {50, 80}, {60, 0}};

float tempProfil[] = {0, 60, 100, 200, 260, 0};
float setpoint = 145, input = 250, output = 200, pidInput = 250;
float kp = 100, ki = 0.001, kd = 10000;
int dummyRot = 0;
float temp;

PID pid(&pidInput, &output, &setpoint, kp, ki, kd, DIRECT);

void rotated(ESPRotary &r) { dummyRot += r.getPosition(); }

extern "C" void app_main();

void app_main() {
  DisplaySSD1306_128x32_I2C display(-1);

  hw_timer_enable(true);
  hw_timer_set_reload(false);

  const char *mainMenuText[] = {"Start1",  "Start2",   "Start3", "Stop",
                                "Monitor", "SetPoint", "About"};
  LcdGfxMenu mainMenu(mainMenuText, sizeof(mainMenuText) / sizeof(char *));
  Encoder encoder;
  PT100 pt100;
  SimpleControl control;
  Drawing drawing(display, pt100);

  encoder.connectedMenu = &mainMenu;

  display.setFixedFont(ssd1306xled_font6x8);
  display.begin();
  display.getInterface().flipHorizontal(1);
  display.getInterface().flipVertical(1);
  display.clear();
  control.sensor = &pt100;

  mainMenu.show(display);

  /* display.printFixed(0, display.height() - 3, "SORE"); */
  /* lcd_delay(10000); */
  control.setPoint = 200;
  uint8_t oldState = 0;
  char buff[50];
  uint16_t adc_raw;
  float ssrOutput = 0;

  pid.setOutput(&ssrOutput);
  pid.SetSampleTime(10);
  pid.SetOutputLimits(0, 255);
  pid.SetTunings(kp, ki, kd);
  /* pid.SetControllerDirection(ctrlDirection); */
  pid.SetMode(AUTOMATIC);

  ESPRotary rot;
  rot.begin(ENC_A, ENC_B, CLICKS_PER_STEP);
  /* rot.setChangedHandler(rotated); */
  int lastIndex = 0;
  int selectedMenu = -1;
  int lastClicked = 0;

  uint32_t mainLoopLastTime = 0;

  for (;;) {

    rot.loop();

    if (selectedMenu != MENU_MONITOR) {
      mainMenu.show(display);
    } else if (selectedMenu == MENU_MONITOR) {
      drawing.drawProfile(tempProfil);
    }

    if (rot.getPosition() > lastIndex) {
      mainMenu.down();
      lastIndex = rot.getPosition();
    } else if (rot.getPosition() < lastIndex) {
      mainMenu.up();
      lastIndex = rot.getPosition();
    }

    // reading toggle button
    bool buttonLevel = !gpio_get_level(ENC_SW);
    if (buttonLevel) {
      lastClicked = buttonLevel;
    }

    if (lastClicked == 1 && buttonLevel == 0) {
      if (selectedMenu != -1) {
        selectedMenu = -1;
        lastClicked = buttonLevel;
      } else {
        display.clear();
        selectedMenu = mainMenu.selection();
        lastClicked = buttonLevel;
      }
    }

    if (selectedMenu != -1) {
      sprintf(buff, "%s", mainMenuText[selectedMenu]);
      display.printFixed(display.width() / 2, display.height() / 2, buff);
    }

    /// CONTROL....
    if (xTaskGetTickCount() > mainLoopLastTime + 10) {
      adc_read(&adc_raw);
      temp = pt100.process();
      pidInput = temp;
      mainLoopLastTime = xTaskGetTickCount();
    }

    /* if (temp > 230) { */
    /*   setpoint = 150; */
    /* } else { */
    /*   setpoint = 200; */
    /* } */

    pid.Compute();
    /* if (temp > setpoint - 50) { */
    /*   gpio_set_level(SSR_PIN, SSR_OFF); */
    /**/
    /* } else { */
    turnOnWithDelay(SSR_PIN, ssrOutput);
    /* } */

    /* sprintf(buff, "t %d r %d sp %d o %d %d p %d d %d", (int)temp, adc_raw, */
    /*         (int)setpoint, (int)ssrOutput, (int)xTaskGetTickCount(), */
    /*         (int)rot.getPosition(), (int)rot.getDirection()); */
    sprintf(buff, "%d", (int)temp);
    display.printFixed(display.width() / 2, display.height() / 3, buff);
  }
}
