#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include <FastLED.h>

/*

*/
class CN_WS{
  public:
    CN_WS();

    // callbacks
    typedef std::function<void(int,int)> ShowNumberFunction;
    ShowNumberFunction _ShowNumberFunction;
    void onShowNumber(ShowNumberFunction cbShowNumber){_ShowNumberFunction = cbShowNumber;}
    void runShowNumber(int i, int c) {if(_ShowNumberFunction) {_ShowNumberFunction(i,c);}}

    typedef std::function<void(long)> SetTimerFunction;
    SetTimerFunction _SetTimerFunction;
    void onSetTimer(SetTimerFunction cb){_SetTimerFunction = cb;}
    void runSetTimer(long i) {if(_SetTimerFunction) {_SetTimerFunction(i);}}

    typedef std::function<void(int,int,int,int,int,int,int)> THandlerFunction;
    THandlerFunction _cbUpdateEvent;
    void onUpdateEvent(THandlerFunction cb){_cbUpdateEvent = cb;}
    void runUpdateEvent(uint8_t s, uint8_t i, uint8_t h, uint8_t dOw, uint8_t d, uint8_t m, uint8_t y) {if(_cbUpdateEvent) {_cbUpdateEvent(s, i, h, dOw, d, m, y);}}

    typedef std::function<void(uint8_t)> SetDisplayModeFunction;
    SetDisplayModeFunction _cbSetDisplayMode;
    void onSetDisplayMode(SetDisplayModeFunction cb){_cbSetDisplayMode = cb;}
    void runSetDisplayMode(uint8_t n){if(_cbSetDisplayMode){_cbSetDisplayMode(n);}}

    typedef std::function<void(uint8_t, bool)> onConfigFunction;
    onConfigFunction _cbSetConfig;
    void onConfig(onConfigFunction cb){_cbSetConfig = cb;}
    void runConfig(uint8_t n, bool write){if(_cbSetConfig){_cbSetConfig(n,write);}}

    typedef std::function<void(bool)> StartDisplayTimersFunction;
    StartDisplayTimersFunction _cbStartDisplayTimers;
    void onStartDisplayTimers(StartDisplayTimersFunction cb){_cbStartDisplayTimers = cb;}
    void runStartDisplayTimers(bool n){if(_cbStartDisplayTimers){_cbStartDisplayTimers(n);}}

    typedef std::function<void(void)> voidFunction;
    voidFunction _cbLightsOff;
    void onLightsOff(voidFunction cb){_cbLightsOff = cb;}
    void runLightsOff(){if(_cbLightsOff){_cbLightsOff();}}

    voidFunction _cbAutoconnect;
    void onAutoconnect(voidFunction cb){_cbAutoconnect = cb;}
    void runAutoconnect(){if(_cbAutoconnect){_cbAutoconnect();}}

    voidFunction _cbUpdateNTP;
    void onUpdateNTP(voidFunction cb){_cbUpdateNTP = cb;}
    void runUpdateNTP(){if(_cbUpdateNTP){_cbUpdateNTP();}}

    typedef std::function<void(uint8_t,uint8_t,int)> SetPixelFunction;
    SetPixelFunction _setPixel;
    void onSetPixel(SetPixelFunction cb){_setPixel = cb;}
    void runSetPixel(uint8_t x, uint8_t y, int v) {if(_setPixel) {_setPixel(x,y,v);}}



    void begin(void);
    void loop(void);
    static void event(uint8_t, WStype_t, uint8_t * ,size_t);
    
    void onWsEvent(uint8_t, WStype_t, uint8_t * ,size_t);
    void onWsEventStr(uint8_t, WStype_t, String);
    
    void sendNow(uint8_t);
    void sendString(String response, uint8_t num = 0);
    WebSocketsServer* wss;
    int displayValues[3]={0,0,0};
    typedef struct{
      uint8_t second;
      uint8_t minute;
      uint8_t hour;
      uint8_t dayOfWeek;
      uint8_t day;
      uint8_t dayOfMonth;
      uint8_t month;
      uint8_t year;
      float temp;
      uint8_t mode;
      int unix;
    } LocalDateTime;
    LocalDateTime now;
    long timer;
    bool commited = true;

    // callback
    typedef std::function<void(void)> onReloadFunction;
    void onReload(onReloadFunction onReloadCB){_RunReloadFunction = onReloadCB;}
    onReloadFunction _RunReloadFunction;
    virtual void RunReloadFunction(void) {if(_RunReloadFunction) {_RunReloadFunction();}}

    /* connect wifi bindings */
    typedef std::function<void(const char*, const char*)> ConnectWifiFunction;
    ConnectWifiFunction _ConnectWifi;
    void onConnectWifi(ConnectWifiFunction cbConnectWifi){_ConnectWifi = cbConnectWifi;}
    void runConnectWifi(String ssid_str, String pass_str){
      const char * ssid = ssid_str.c_str();
      const char * pass = pass_str.c_str();
      if(_ConnectWifi) _ConnectWifi(ssid, pass);
      }

    void internalMessage(String m){
//      uint8_t pl[m.length()];
//      const char* plc = m.c_str();
//      for(int i=0; i<m.length(); i++) pl[i] = plc[i];
      onWsEventStr(0, WStype_TEXT, m);
      }
    
    
  };
