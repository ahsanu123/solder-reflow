#include "PT100.h"
#define CONTROL_TOLERANCE 3.0
#define CONTROL_MINIMUM_TEMP 160

enum ControlState { IDLE = 0, HEATING, MAINTAIN_HEAT, COOLDOWN };

class SimpleControl {

public:
  float setPoint;
  ControlState state;
  PT100 *sensor;

  int process();

  SimpleControl() {
    setPoint = CONTROL_MINIMUM_TEMP;
    state = IDLE;
  }

  SimpleControl(float setPoint, ControlState state) {
    setPoint = setPoint;
    state = state;
  }
};
