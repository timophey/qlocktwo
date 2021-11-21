#include "FS.h"
#include <EEPROM.h>
#include <stdint.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>


class CL_Config{
  unsigned char a0 = 0x03;
  unsigned char a1 = 0x17;
  public:
    void save(uint8_t index);
    void load(uint8_t index);
    char* filename(uint8_t index);

  // callbacks
  typedef std::function<void(void)> onReloadFunction;
  void onReload(onReloadFunction onReloadCB){_RunReloadFunction = onReloadCB;}
  onReloadFunction _RunReloadFunction;
  virtual void RunReloadFunction(void) {if(_RunReloadFunction) {_RunReloadFunction();}}
    
  };
