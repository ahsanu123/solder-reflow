
#ifndef TESTER_HEATER
#define TESTER_HEATER

/**
 * COPIED: https://gist.github.com/bradley219/5373998
 */

#include <cmath>

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

struct PIDReturnData_t {
  double error;
  double integral;
  double derivative;
  double proportional;
  double setpoint;
  double output;
  double realOutput;
};

class PID {
public:
  ~PID();
  PID(double dt, double max, double min, double Kp, double Kd, double Ki)
      : _dt(dt), _max(max), _min(min), _Kp(Kp), _Kd(Kd), _Ki(Ki), _pre_error(0), _integral(0) {}

  double calculate(double setpoint, double pv) {

    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = _Kp * error;

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    // Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout       = _Kd * derivative;

    // Calculate total output
    double output      = Pout + Iout + Dout;

    double mapedOutput = map(output, -15, 15, _min, _max);

    // Restrict to max/min
    if (mapedOutput > _max)
      mapedOutput = _min;
    if (mapedOutput < _min)
      mapedOutput = _max;

    // Save error to previous error
    _pre_error     = error;

    this->pidInfos = {
      .error        = error,
      .integral     = Iout,
      .derivative   = derivative,
      .proportional = Pout,
      .setpoint     = setpoint,
      .output       = mapedOutput,
      .realOutput   = output,
    };

    return mapedOutput;
  }

  PIDReturnData_t getDebugInfo() { return this->pidInfos; }
  void            setDirection(bool value) { this->_direction = value; }

private:
  double          _dt;
  double          _max;
  double          _min;
  double          _Kp;
  double          _Kd;
  double          _Ki;
  double          _pre_error;
  double          _integral;
  bool            _direction = false;

  PIDReturnData_t pidInfos;
};

#endif //
