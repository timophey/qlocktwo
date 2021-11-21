
#include <Wire.h>
#include <uRTCLib.h>
#include "Ticker.h"
#include <ESPDateTime.h>

#define DS3231_SDA 4
#define DS3231_SCL 5

class CL_Wire{
  public:
    void begin(void);
    void loop(void);
    void i2c_scan(void);
    void i2c_found(uint8_t);
    void i2c_rtc_serialprint(void);
    void i2c_rtc_settime(int s, int i, int h, int dOw, int d, int m, int y);
    void i2c_rtc_update(void);
    void esp_ntp_start(long);
    void esp_ntp_log();
    //
    int getHours();
    int getMinutes();
    int getSeconds();
    //

    uint8_t i2c_rtc_addr;
    uRTCLib* rtc;
    Ticker _Ticker;
  };
