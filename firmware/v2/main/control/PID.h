
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
  double unBoundedOutput;
};

class PID {
public:
  ~PID();
  PID(double dt, double max, double min, double Kp, double Kd, double Ki)
      : _timeSpan(dt), _maximumDriverValue(max), _minimumDriverValue(min), _constantProportional(Kp),
        _constantDerivation(Kd), _constantIntegration(Ki), _lastPlanValue(0), _integralValue(0) {}

  double calculate(double setPoint, double error, double plantValue) {

    // Proportional term
    double proportionalOut = _constantProportional * error;
    if (proportionalOut < 0)
      proportionalOut *= -1;

    // Integral term
    _integralValue += error;
    double integralOut = _constantIntegration * _integralValue;

    if (integralOut > _maximumDriverValue)
      integralOut = _maximumDriverValue;

    // Derivative term
    double derivative    = (plantValue - _lastPlanValue) / _timeSpan;
    double derivativeOut = _constantDerivation * derivative;

    // Calculate total output
    double output        = proportionalOut + integralOut + derivativeOut;
    double boundedOutput = output;

    // Restrict to max/min
    if (output > _maximumDriverValue)
      boundedOutput = _maximumDriverValue;
    if (output < _minimumDriverValue)
      boundedOutput = _minimumDriverValue;

    // Save error to previous error
    _lastPlanValue  = plantValue;

    this->_pidInfos = {
      .error           = error,
      .integral        = integralOut,
      .derivative      = derivativeOut,
      .proportional    = proportionalOut,
      .setpoint        = setPoint,
      .output          = boundedOutput,
      .unBoundedOutput = output,
    };

    return boundedOutput;
  }

  PIDReturnData_t getDebugInfo() { return this->_pidInfos; }
  void            setDirection(bool value) { this->_direction = value; }

private:
  double          _timeSpan;
  double          _maximumDriverValue;
  double          _minimumDriverValue;
  double          _constantProportional;
  double          _constantDerivation;
  double          _constantIntegration;
  double          _lastPlanValue;
  double          _integralValue;
  bool            _direction = false;

  PIDReturnData_t _pidInfos;
};

#endif //
