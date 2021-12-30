#include "CL_UDP.h"


//void CL_UDP::CL_UDP(){}

void CL_UDP::begin(){
  const char* NTPserver_attach = NTPserver.c_str();//"192.168.1.237";//

  // Read NTP offset from EEPROM
  signed char utcOffsetEEPROM = EEPROM.read(0x04); // [-24...+24]
  if(utcOffsetEEPROM > -25 && utcOffsetEEPROM < 25){
    utcOffsetInSeconds = utcOffsetEEPROM * 1800;
    }
  
  this->timeClient = new NTPClient(ntpUDP, NTPserver_attach, utcOffsetInSeconds);
  this->timeClient->begin();
  Serial.println("NTP client started");
  //  update();
  //_Ticker.attach_ms_scheduled(11 * 60 * 1000, std::bind(&CL_UDP::update, this));

  if(WiFi.status() == 3){
      if(ota_enabled) setup_ota();
    }else{
      Serial.println("OTA server disabled");
      dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
      dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    }
  
  }

void CL_UDP::setNTP(String s){if(strlen(s.c_str())>0) NTPserver = s;}
void CL_UDP::setUTCOffset(int s){utcOffsetInSeconds = s;}

void CL_UDP::setup_ota(){
  
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");  //  "Начало OTA-апдейта"
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");  //  "Завершение OTA-апдейта"
  });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    // display vars
    int nx = 6;int values[nx];
    int prpcd = progress / (total / 100);
    Serial.printf("OTA Progress: %u%%\r", prpcd);
    runShowProgress(prpcd);
//    for(int i=0; i<nx; i++){int n = nx-(i+1);values[n] = prpcd % 10;prpcd /= 10;}
//    for(int i=0; i<nx; i++){this->runSetLed(i,values[i],0xFFFFFF);}
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    //  "Ошибка при аутентификации"
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    //  "Ошибка при начале OTA-апдейта"
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    //  "Ошибка при подключении"
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    //  "Ошибка при получении данных"
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
    //  "Ошибка при завершении OTA-апдейта"
  });
  //ArduinoOTA.setHostname("esp8266_OTA_40");
  ArduinoOTA.begin();
  Serial.println("ArduinoOTA Ready");  //  "Готово"  
  
  }

void CL_UDP::loop(){
    if(ota_enabled){
      ArduinoOTA.handle();
    }
    dnsServer.processNextRequest();
    /*int interval = 11 * 60 * 1000;
    if( millis() % interval == 0){
      update();
    }*/

  }

bool CL_UDP::update(){
  if(WiFi.status() != WL_CONNECTED) return false;
  Serial.print("timeClient.update from ");Serial.println(NTPserver);
  //return;
    long startAt = millis();
    if(this->timeClient->update()){
    long msTaken = millis() - startAt;
//    if(msTaken > 0){
      Serial.print(daysOfTheWeek[this->timeClient->getDay()]);
      Serial.print(", ");
      /*Serial.print(this->timeClient->getHours());
      Serial.print(":");
      Serial.print(this->timeClient->getMinutes());
      Serial.print(":");
      Serial.println(this->timeClient->getSeconds());*/
      Serial.println(this->timeClient->getFormattedTime());
      Serial.println(this->timeClient->getFormattedDate());
  //    Serial.println("timeClient.update done");
      Serial.print("NTP client updated in ");Serial.print(msTaken);Serial.println("ms");
      update_event();
      return true;
//      }
    }else{
      Serial.println("NTP client update failed");
      return false;
    }
  return false;
  }

void CL_UDP::update_event(){
  // http://digitrode.ru/computing-devices/mcu_cpu/2094-internet-chasy-svoimi-rukami-na-osnove-esp8266-i-protokola-ntp.html
  // https://codeload.github.com/taranais/NTPClient/zip/master
  String dateFormatted = this->timeClient->getFormattedDate();
  int ntp_y = dateFormatted.substring(0, 4).toInt() % 100;
  int ntp_m = dateFormatted.substring(5, 7).toInt();
  int ntp_d = dateFormatted.substring(8, 10).toInt();
  runUpdateEvent(
    this->timeClient->getSeconds(),
    this->timeClient->getMinutes(),
    this->timeClient->getHours(),
    this->timeClient->getDay(),//+1
    ntp_d,ntp_m,ntp_y
    );
  DateTime.setTime(this->timeClient->getEpochTime());
  }

void CL_UDP::onUpdateEvent(THandlerFunction cbUpdateEvent){
  _cbUpdateEvent = cbUpdateEvent;
  }
