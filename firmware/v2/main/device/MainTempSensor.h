

#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif

class TempDevice {
public:
  virtual float    GetTemp()    = 0;
  virtual uint16_t GetRawAdc()  = 0;
  virtual bool     InitDevice() = 0;
};

class MainTempSensor {
  TempDevice *device;

public:
  float GetTemperatureInC();
  float GetTemperatureInF();
  float GetTemperatureInR();
};

#ifdef __cplusplus
}
#endif
