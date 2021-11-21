#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <stdlib.h>
#define DBG_OUTPUT_PORT Serial

class CN_WebServer{
  
  public:
    CN_WebServer();
    void begin(void);
    void loop(void);
    void handleRoot(void);
    void handleFileList(void);
    void handleFileCreate(void);
    void handleFileDelete(void);
    void handleFileUpload(void);
    void handleFirmwareUpdate(void);
    bool handleFileRead(String);
    void handleWiFiStatus(void);
    void handleSysInfo(void);
    String getContentType(String);
    void handleWifiConnections(void);

    bool isOnline(void);
    
    ESP8266WebServer* httpd;
    File fsUploadFile;
    int load=0;
    bool cache_enabled = true;
    bool stand_mode = false;

    // ShowNumberBind
    typedef std::function<void(int,int,int)> ShowNumberFunction;
    ShowNumberFunction _ShowNumberFunction;
    void onShowNumber(ShowNumberFunction cbShowNumber){_ShowNumberFunction = cbShowNumber;}
    void runShowNumber(int i, int d, int c) {if(_ShowNumberFunction) {_ShowNumberFunction(i,d,c);}}

    // stopAllCallback
    typedef std::function<void()> stopAllCallback;
    stopAllCallback _stopAllFunction;
    void onStopAll(stopAllCallback cbStopAll){_stopAllFunction = cbStopAll;}
    void runStopAll() {if(_stopAllFunction) {_stopAllFunction();}}

    // captive portal
    boolean captivePortal(void);
    boolean isIp(String);
    String toStringIp(IPAddress);
    const char *myHostname = "esp8266";

    // callbacks
    typedef std::function<void(int)> ShowProgressFunction;
    ShowProgressFunction _ShowProgressFunction;
    void onShowProgress(ShowProgressFunction cbShowNumber){_ShowProgressFunction = cbShowNumber;}
    void runShowProgress(int i) {if(_ShowProgressFunction) {_ShowProgressFunction(i);}}


    
  protected:
    void setup(void);
    
    
  };
