#define FTP_DEBUG true;
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266FtpServerLittleFS.h>
#include "CL_UDP.h"
#include "CN_WebServer.h"
#include "CN_WS.h"
#include <FastLED.h>
#define NUM_LEDS    11*10

class CL_Wifi{
  public:
    CL_Wifi();
    void loop(void);
    void begin(void);
    void autoconnect(void);
    void connect(const char*,const char*, bool ac = false);
    void softAP(const char*,const char*);
    void softAPup();
    void startftp(String ftp_user="esp8266",String ftp_pass="esp8266");
    void showLocalIP(IPAddress ip);

    bool ftp_enabled = true;
//    bool ota_enabled = true;
    bool spftAP_keep_up = false;
    bool spftAP_opened = false;
    bool spftAP_hidden = false;

    CL_UDP UDP;
    CN_WebServer Server80;
    CN_WS Server81;
    FtpServer Server21;

    IPAddress localIPaddr;

    typedef std::function<void(int,int)> SetLedFunction;
    SetLedFunction _SetLedFunction;
    void onSetLed(SetLedFunction cbSetLed){_SetLedFunction = cbSetLed;}
    virtual void runSetLed(int i, int c) {if(_SetLedFunction) {_SetLedFunction(i,c);}}

    // callbacks
    typedef std::function<void(int,int)> ShowNumberFunction;
    ShowNumberFunction _ShowNumberFunction;
    void onShowNumber(ShowNumberFunction cbShowNumber){_ShowNumberFunction = cbShowNumber;}
    void runShowNumber(int i, int c) {if(_ShowNumberFunction) {_ShowNumberFunction(i,c);}}

    typedef std::function<void(void)> voidFunction;
    voidFunction _cbLightsOff;
    void onLightsOff(voidFunction cb){_cbLightsOff = cb;}
    void runLightsOff(){if(_cbLightsOff){_cbLightsOff();}}

    // callbacks
    typedef std::function<void(int)> ShowProgressFunction;
    ShowProgressFunction _ShowProgressFunction;
    void onShowProgress(ShowProgressFunction cbShowNumber){_ShowProgressFunction = cbShowNumber;}
    void showProgress(int i) {if(_ShowProgressFunction) {_ShowProgressFunction(i);}}


  protected:
    // connect
    const char* ap_ssid_default = "esp8266";
    const char* ap_pass_default = "12345678";


  };
