#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESPDateTime.h>
#include <string>

using namespace std;
//#include <TimeLib.h>
#include "Ticker.h"

class CL_UDP{
  typedef void (*CL_UDPEvent)(int s, int i, int h, int dOw, int d, int m, int y);
  typedef std::function<void(int,int,int,int,int,int,int)> THandlerFunction;
  public :
    //void CL_UDP(void);
    void begin(void);
    void setNTP(String);
    void setUTCOffset(int);
    void setup_ota(void);
    void loop(void);
    bool update(void);
    void update_event(void);
    void onUpdateEvent(THandlerFunction cbEvent);
    //const char* NTPserver = "pool.ntp.org";
    String NTPserver = "pool.ntp.org";
    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    long utcOffsetInSeconds = 3600 * 5;
    WiFiUDP ntpUDP;
    NTPClient* timeClient;
    Ticker _Ticker;
    
    const uint8_t DNS_PORT = 53;
    DNSServer dnsServer;
    bool ota_enabled = true;

    // SetLedBind
    typedef std::function<void(int,int,int)> SetLedFunction;
    SetLedFunction _SetLedFunction;
    void onSetLed(SetLedFunction cbSetLed){_SetLedFunction = cbSetLed;}
    void runSetLed(int i, int d, int c) {if(_SetLedFunction) {_SetLedFunction(i,d,c);}}

    // callback progress
    typedef std::function<void(int)> ShowProgressFunction;
    ShowProgressFunction _ShowProgressFunction;
    void onShowProgress(ShowProgressFunction cbShowNumber){_ShowProgressFunction = cbShowNumber;}
    void runShowProgress(int i) {if(_ShowProgressFunction) {_ShowProgressFunction(i);}}

    // callback
//    typedef std::function<void(int)> ShowProgressFunction;
//    ShowProgressFunction _ShowProgressFunction;
//    void onShowProgress(ShowProgressFunction cbShowNumber){_ShowProgressFunction = cbShowNumber;}
//    void runShowProgress(int i) {if(_ShowProgressFunction) {_ShowProgressFunction(i);}}

  
  // callbacks
  protected: 
    THandlerFunction _cbUpdateEvent;
    virtual void runUpdateEvent(int s, int i, int h, int dOw, int d, int m, int y) {if(_cbUpdateEvent) {_cbUpdateEvent(s, i, h, dOw, d, m, y);}}

    
  };
