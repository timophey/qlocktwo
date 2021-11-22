#include <EEPROM.h>
#include <FastLED.h>
#include "CL_Wifi.h"
#include "CL_Wire.h"
#include "CL_Term.h"
#include "CL_Display.h";
#include "Ticker.h"
#include <stdlib.h>
#include "CL_Config.h";

class CL_App{
  public:
      CL_App();
      void setup(void);
      void loop(void);
      void demo(void);
      void setTime(int,int,int,int,int,int,int);
      void setLed(int,int);
      void showTimeWords(void);
      void showNumberTest();
      void shareWSnow(void);

      void setDisplayTimer(void);
      void setSwitchTimer(void);
      void stopDisplayTimer(void);
      void reLoadHandler(void);

      Ticker _Ticker500;
      Ticker _Ticker1000;

      void scheduled500(void);
      void scheduled1000(void);

      CL_Display* Display;
      CL_Wifi* Network;
      CL_Term Term;
      CL_Wire i2c;
      CL_Config* Config;
      
      bool ss = false;
      bool st = false;
      uint8_t numb = 0;

      int _switchDelay = 5000;
  };

/*

EEPROM map

0x01 - Wifi settings byte
  0: Enable FTP
  1: Enable OTA
  4: 'softAP opened',
  5: 'softAP hidden',
  6: 'softAP keep up',
  7: Stand DEMO mode

0x02 - webserver config byte
  2: Enable Cache for HTTP
  3: Enable CaptivePortal
  4: Allow EDIT action
  5: Allow WEB Upgrade

0x03 - led strip config byte
  0: use HSV
  1: delta < Value
  2: delta > Value
  3: fall to 0
  4: fall to 255
  5: bounce
  7: random Hue
  

0x04 - Timezone


Color
0x0D = 13 - Hue
0x0E = 14 - Sat
0x0F = 15 - Val
0x10 = 16 - Red  
0x11 = 17 - Green 
0x12 = 18 - Blue

0x13 = 19 - DrawDown Deep
0x14 = 20 - DrawDown Step

0x15 = 21 - Switch Delay
0x16 = 22 - Print Delay
0x17 = 23 - Candle Delay

0xD8 = 216 - Curent config Loaded


*/