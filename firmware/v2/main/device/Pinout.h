
#include <string>
using namespace std;

#ifndef INCLUDE_PINOUT
#define INCLUDE_PINOUT

// IO NUMBER | TYPE | Function
#define DEV_IO0 IO0
#define DEV_RXD IO1
#define DEV_LCD_CS IO2
#define DEV_TXD IO3
#define DEV_SSRIN1 IO4
#define DEV_VSPI_CS0 IO5
#define DEV_MTDI IO12
#define DEV_MTCK IO13
#define DEV_MTMS IO14
#define DEV_MTDO IO15
#define DEV_I2C_SDA IO16
#define DEV_I2C_SCL IO17
#define DEV_VSPI_CLK IO18
#define DEV_VSPI_Q IO19
#define DEV_SSRIN2 IO21
#define DEV_IN4 IO22
#define DEV_VSPI_D IO23
#define DEV_IN3 IO25
#define DEV_IN2 IO26
#define DEV_IN1 IO27
#define DEV_LCD_RST IO32
#define DEV_LCD_DC IO33
#define DEV_PT100 IO34
#define DEV_ADC1_CH7 IO35

enum device_pinout_type_t { INPUT, OUTPUT, IO, POWER };

typedef struct {
  string               name;
  device_pinout_type_t type;
} device_pinout_t;

#endif
