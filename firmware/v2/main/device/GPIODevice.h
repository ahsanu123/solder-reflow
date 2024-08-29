
#ifndef INCLUDE_GPIO_DEVICE
#define INCLUDE_GPIO_DEVICE

#include "driver/gpio.h"
#include "soc/gpio_num.h"
#include <array>
#include <cstddef>

#define HIGH true
#define LOW false
#define NUM_OF_BUTTONS 5

#define INPUT1 gpio_num_t::GPIO_NUM_27
#define INPUT2 gpio_num_t::GPIO_NUM_26
#define INPUT3 gpio_num_t::GPIO_NUM_25
#define INPUT4 gpio_num_t::GPIO_NUM_22
#define OUTPUT1 GPIO_NUM_4
#define OUTPUT2 GPIO_NUM_21

enum eRegisCallbackStatus { Fail = 0, Success = 1 };

typedef void (*Callback)();
typedef bool (*ReadPin)(gpio_num_t &pin);
typedef void (*InitGpiosCallback)(std::array<gpio_num_t, NUM_OF_BUTTONS> pins);

typedef struct {
  bool currentState;
  bool lastState;
  gpio_num_t pin;
  Callback onPressIn;
  Callback onPressOut;
} ButtonStruct;

ButtonStruct arrayButton[NUM_OF_BUTTONS] = {
    [0] =
        {
            .currentState = HIGH,
            .lastState = HIGH,
            .pin = gpio_num_t::GPIO_NUM_27,
            .onPressOut = NULL,
            .onPressIn = NULL,
        },
    [1] =
        {
            .currentState = HIGH,
            .lastState = HIGH,
            .pin = gpio_num_t::GPIO_NUM_26,
            .onPressOut = NULL,
            .onPressIn = NULL,
        },
    [2] =
        {
            .currentState = HIGH,
            .lastState = HIGH,
            .pin = gpio_num_t::GPIO_NUM_25,
            .onPressOut = NULL,
            .onPressIn = NULL,
        },
    [3] =
        {
            .currentState = HIGH,
            .lastState = HIGH,
            .pin = gpio_num_t::GPIO_NUM_22,
            .onPressOut = NULL,
            .onPressIn = NULL,
        },
    [4] =
        {
            .currentState = HIGH,
            .lastState = HIGH,
            .pin = gpio_num_t::GPIO_NUM_2,
            .onPressOut = NULL,
            .onPressIn = NULL,
        },
};

std::array<ButtonStruct, NUM_OF_BUTTONS> defaultButtonLists = {
    arrayButton[0], arrayButton[1], arrayButton[2],
    arrayButton[3], arrayButton[4],
};

void defaultInitGpioFunction(std::array<gpio_num_t, NUM_OF_BUTTONS> pins) {
  gpio_config_t ioConfig = {};

  ioConfig.mode = GPIO_MODE_INPUT;
  ioConfig.intr_type = GPIO_INTR_DISABLE;
  ioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
  ioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;

  for (auto &pin : pins) {
    ioConfig.pin_bit_mask += (1ULL << pin);
  }
  gpio_config(&ioConfig);
}

bool defaultReadPinCallback(gpio_num_t &pin) {
  bool retVal = (bool)gpio_get_level(pin);
  return retVal;
}

class Button {
private:
  std::array<ButtonStruct, NUM_OF_BUTTONS> listButtons;
  ReadPin readPinCallback;

public:
  Button(std::array<ButtonStruct, NUM_OF_BUTTONS> buttons = defaultButtonLists,
         ReadPin readPinCallback = defaultReadPinCallback) {

    this->readPinCallback = readPinCallback;
    this->listButtons = buttons;

    for (auto &button : this->listButtons) {
      button.onPressIn = NULL;
      button.onPressOut = NULL;
      button.lastState = true;
      button.currentState = true;
    }
  }

  void Init(InitGpiosCallback initFunction = defaultInitGpioFunction) {

    std::array<gpio_num_t, NUM_OF_BUTTONS> pins;
    int index = 0;

    for (auto &button : this->listButtons) {
      pins.at(index) = button.pin;
      index++;
    }

    initFunction(pins);
  }

  void Scan() {
    for (auto &button : this->listButtons) {
      button.currentState = this->readPinCallback(button.pin);

      if (button.lastState == HIGH && button.currentState == LOW) {
        if (button.onPressIn != NULL) {
          button.onPressIn();
        }

        button.lastState = button.currentState;
      }

      if (button.lastState == LOW && button.currentState == HIGH) {
        if (button.onPressOut != NULL) {
          button.onPressOut();
        }

        button.lastState = button.currentState;
      }
    }
  }

  eRegisCallbackStatus SetOnPressInCallback(Callback onPressIn, int index) {
    if (index >= 0 && index < NUM_OF_BUTTONS) {
      this->listButtons.at(index).onPressIn = onPressIn;

      return eRegisCallbackStatus::Success;
    }
    return eRegisCallbackStatus::Fail;
  }

  eRegisCallbackStatus SetOnPressOutCallback(Callback onPressOut, int index) {
    if (index >= 0 && index < NUM_OF_BUTTONS) {
      this->listButtons.at(index).onPressOut = onPressOut;

      return eRegisCallbackStatus::Success;
    }
    return eRegisCallbackStatus::Fail;
  }
};

#endif
