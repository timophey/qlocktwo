#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>

class CL_Term{
  public:
    CL_Term(void);
    void loop(void);

    /* connect wifi bindings */

    typedef std::function<void(const char*, const char*)> ConnectWifiFunction;
    ConnectWifiFunction _ConnectWifi;
    void onConnectWifi(ConnectWifiFunction cbConnectWifi){_ConnectWifi = cbConnectWifi;}
    void runConnectWifi(String ssid_str, String pass_str){
      const char * ssid = ssid_str.c_str();
      const char * pass = pass_str.c_str();
      if(_ConnectWifi){
         _ConnectWifi(ssid, pass);
         }
      }


    /* date bind */

    typedef std::function<void()> VoidFunction;
    VoidFunction _DateShow;
    void  onDateShow(VoidFunction cbDateShow){_DateShow = cbDateShow;}
    void runDateShow(){if(_DateShow) _DateShow();}
    
    
    /*  */
  protected:
    int mode = 0x00;
  };
