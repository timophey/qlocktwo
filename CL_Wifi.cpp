#include "CL_Wifi.h"
using namespace std::placeholders;

CL_Wifi::CL_Wifi(){} 

void CL_Wifi::begin(){

  /*
   * Bind actions via WS Server
   */
   this->Server81.onConnectWifi([this](const char* u,const char* p){this->connect(u,p);delay(5);});
//   this->Server81.onConnectWifi(std::bind(&CL_Wifi::connect,this, _1, _2));//  [this](const char* u,const char* p){this->connect(u,p);delay(5);});
   this->Server81.onAutoconnect(std::bind(&CL_Wifi::autoconnect,this));
   this->Server81.onUpdateNTP([this](){
    signed char utcOffsetEEPROM = EEPROM.read(0x04);
    this->UDP.utcOffsetInSeconds = utcOffsetEEPROM * 1800;
    this->UDP.timeClient->setTimeOffset(this->UDP.utcOffsetInSeconds);
    this->UDP.update();
    });
  /* Start Connection
   * */

  uint8_t g = EEPROM.read(0x01);
  ftp_enabled = (bitRead(g, 0));
  //ota_enabled = (bitRead(g, 1));
  UDP.ota_enabled = (bitRead(g, 1));
  spftAP_keep_up = (bitRead(g, 6));
  spftAP_opened = (bitRead(g, 4));
  spftAP_hidden = (bitRead(g, 5));

  
  
//  for(uint8_t i=0; i<8; i++){
//    Serial.printf("geometry[%d]=%d \n\r",i,bitRead(g, i));
//    }

  WiFi.mode(WIFI_STA);
  this->autoconnect();

  if(WiFi.status() != WL_CONNECTED || spftAP_keep_up){
    this->softAPup();
    }


  /* Start Services
   * */
  UDP.onShowProgress([this](uint8_t i){ this->showProgress(i); });
  UDP.begin();
  Server80.begin();
  Server80.onShowProgress([this](uint8_t i){ this->showProgress(i); });
  Server81.begin();
  if(ftp_enabled){
    startftp("esp8266","esp8266");
  }else{
    Serial.println("FTP server disabled");
    }
    
  if(WiFi.status() == WL_CONNECTED){
    /** update time from NTP */
    this->UDP.update();
    }
  }

void CL_Wifi::autoconnect(){
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi is connected");
    localIPaddr = WiFi.localIP();
    Serial.print("Local address: ");Serial.print("http://");
    Serial.print(localIPaddr);Serial.println("/");
    showLocalIP(localIPaddr);
    return;
    }
  /* Scan WiFi and try to connect by config */
  delay(2000);
  uint8_t n = WiFi.scanNetworks();
  if(n > 0){
    Serial.printf("%d Networks found\n",n);

    /* Checked saved in memory first */
    struct station_config conf;
    wifi_station_get_config(&conf);
    const char* conf_ssid = (const char*) conf.ssid;
    const char* conf_pass = (const char*) conf.password;
    for (int i = 0; i < n; ++i){
      if(WiFi.SSID(i) == String(conf_ssid)){
        Serial.println(String()+"CL_Wifi::autoconnect: "+conf_ssid+" saved in wifi_station_get_config(), connect:");
        this->connect(conf_ssid,conf_pass);
        if(WiFi.status() == WL_CONNECTED){
          break;
          }
        }
      }
    if(WiFi.status() != WL_CONNECTED){
      for (int i = 0; i < n; ++i){
        String cn = WiFi.SSID(i);
        String cf = "/wifi_connection_"+cn+".txt";
        Serial.println(String()+"Check "+cn);
        if(LittleFS.exists(cf)){
          File f = LittleFS.open(cf, "r"); String pwd; while (f.available()) pwd += char(f.read()); f.close();
          Serial.println(String()+"CL_Wifi::autoconnect: "+cn+" has config, connect:");
          this->connect(cn.c_str(),pwd.c_str());
          if(WiFi.status() == WL_CONNECTED) break;
          }
        // if connected, exit foreach
        if(WiFi.status() == WL_CONNECTED){
          break;
          }
        }
      }
    }

  
  delay(5);
  }

void CL_Wifi::softAPup(){
    Serial.print("WiFi.status() = ");Serial.println(WiFi.status());
    Serial.println("Wifi AP not configured, use default values");
    //WiFi.disconnect();
    //WiFi.softAPdisconnect();
    WiFi.mode(WIFI_AP_STA);
    char ap_ssid_default_tmp[16];//Qlocktwo 
    sprintf(ap_ssid_default_tmp,  "Qlocktwo-%06x", ESP.getChipId());
    const char* ap_ssid_now = ap_ssid_default_tmp;
//    this->softAP(ap_ssid_now,ap_pass_default);
    this->softAP(ap_ssid_now,ap_pass_default);
  }


void CL_Wifi::connect(const char* sta_ssid,const char* sta_pass, bool ac){
  // Disconnect if connected
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("Disconnect from ");Serial.println(WiFi.SSID());
    //WiFi.mode(WIFI_STA);delay(50);
    WiFi.disconnect();delay(50);
    }
  // Connect again
  Serial.print("Connecting to ");Serial.print(sta_ssid);
  Serial.print(" with password: ");Serial.print(sta_pass);Serial.println();
  WiFi.begin(sta_ssid, sta_pass);
  int WL_CONNECT_TIMEOUT = 30;
  bool s1=false;
//  CRGB c;
  unsigned long startTime = millis();
  float x = (millis() % 1000) / 1000 * 3.14;
  while (WiFi.status() != WL_CONNECTED){
    //Serial.printf("WiFi.status(): %d\n",WiFi.status());
    delay(500);Serial.print(".");s1=!s1; // это счётчик, чтобы при загрузке первая лампа мигала
    runSetLed(9,(s1) ? 0xB0C4DE : 0x000000);//CRGB::LightSkyBlue : CRGB::Black
    if((millis() - startTime) / (WL_CONNECT_TIMEOUT*1000) > 0){
      Serial.println("f*ck");
      break;
      }
  }
//  runSetLed(0,10,0x000000);
  delay(500);
  Serial.printf("WiFi.status(): %d\n\r",WiFi.status());

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    localIPaddr = WiFi.localIP();
    Serial.print("Local address: ");Serial.print("http://");
    Serial.print(localIPaddr);Serial.println("/");
    Serial.println("WiFi connected");
    showLocalIP(localIPaddr);
    /* save to password to text file */
    String cf = "/wifi_connection_"; cf+= sta_ssid; cf+= ".txt";
    if(!LittleFS.exists(cf) && !ac){
      Serial.println("Connection config not exists, create it!");
      File f = LittleFS.open(cf, "w");
      f.print(sta_pass);
      f.close();
      }

  }

  // display result
  int resultColor = (WiFi.status() == WL_CONNECTED) ? 0x00FF00 : 0xFF0000;
//  for(int i=0; i<6; i++){runSetLed(i,10,resultColor);delay(100);}

}

void CL_Wifi::softAP(const char* ap_ssid,const char* ap_pass){
  Serial.print("Wifi AP starts width SSID: ");Serial.print(ap_ssid);Serial.print(" and pass: ");Serial.println(ap_pass);
  Serial.print("Wifi AP Setting... ");

  /*
  WiFi.softAP(ssid, password, channel, hidden, max_connection)
  */
  bool started = WiFi.softAP(ap_ssid, (spftAP_opened)?"":ap_pass, 1, spftAP_hidden, 8);
  
  Serial.println(started ? "Ready" : "Failed!");
  Serial.print("Wifi AP IP address: ");
  Serial.println(WiFi.softAPIP());
  showLocalIP(WiFi.softAPIP());
  }


void CL_Wifi::startftp(String ftp_user,String ftp_pass){
  bool ftp_enabled = true;//this->config_wifi["ftp_enabled"].as<bool>();
  Server21.begin(ftp_user,ftp_pass);
  String message = "FTP server started with user: "+ftp_user+" and pass: "+ftp_pass;
  Serial.println(message);
  }

void CL_Wifi::showLocalIP(IPAddress ip){
  if(!bitRead(EEPROM.read(0x01), 2)) return;
//  Serial.print("CN_Wifi::showLocalIP ");
//   очистить поле
//  for(uint8_t i=0; i<NUM_LEDS; i++) leds[i] = CRGB::Black; FastLED.show();
  for(int i=0; i<4; i++){ // перебираем 4 части
    int p = ip[i];
    int digs[3];
    int cs[4] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White};
    for(int j=0; j<3; j++){
      digs[j] = p % 10; p=p/10;
      }

    int tail = 0;
    for(int j=0; j<3; j++){
      int d = digs[2-j];
        tail+=d;if(!tail) continue;
        runShowNumber(d,cs[i]);
        Serial.print(d);
        delay(1280);
      }
    runLightsOff();
    uint8_t l = 98; runSetLed(l+i,cs[i]);
    Serial.print(".");
    delay(768);
//        break;
  }
  Serial.println(";");
  delay(2048);
}

void CL_Wifi::loop(){
  
  Server80.loop();
  Server81.loop();
  UDP.loop();
  if(ftp_enabled) Server21.handleFTP();
  
  }
