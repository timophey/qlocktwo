#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

class CL_Cron{
  public:
    CL_Cron();
    uint8_t i;
    uint8_t h;
    uint8_t d;
    uint8_t m;
    uint8_t w;
    String tkeys[5] = {"w","m","d","h","i"};
    void setNow(uint8_t i, uint8_t h, uint8_t d, uint8_t m, uint8_t w);
    bool trigger();
    bool match(String compare, uint8_t value);
    
    uint8_t values[5];
    uint8_t valuesNew[5];
//    DynamicJsonDocument config;

  typedef std::function<void(String)> onCmdFunction;
  onCmdFunction _RunCmd;
  void onRunCmd(onCmdFunction cb){_RunCmd = cb;}
  virtual void RunCmd(String m) {
    if(_RunCmd){_RunCmd(m);}
    }

    
  };
