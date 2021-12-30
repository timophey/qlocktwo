#include "CL_App.h"

#include <ESPDateTime.h>

time_t myTimeCallback() {
  Serial.println("myTimeCallback");
  Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
  return 1455451200; // UNIX timestamp
}

using namespace std::placeholders;

  
CL_App::CL_App(){}
void CL_App::setup(){
  delay(2550);
  // start hardware
  Serial.begin(115200);
  EEPROM.begin(512);

  // boot
  Serial.println();
  Serial.println("Qlocktwo by @tima_tey");
  Serial.printf("Build date: %s, time: %s\n\r",__DATE__,__TIME__);

  Serial.print("Mounting FS... ");
  
  LittleFS.begin();
  LittleFS.setTimeCallback(myTimeCallback);
  Serial.println("OK");

  this->Config = new CL_Config();
  this->Config->onReload(std::bind(&CL_App::reLoadHandler, this));

  /* Bindings
   * */
  this->Network = new CL_Wifi();
  this->Network->UDP.onUpdateEvent(std::bind(&CL_App::setTime, this, _1, _2, _3, _4, _5, _6, _7));
  this->Network->onLightsOff([this](){ this->Display->lightsDown(); });
  this->Network->onShowNumber([this](int i, int color){
    this->Display->lightsDown();
    if(color > 0) this->Display->main_color = color;
    this->Display->showTimeWords(i,0);
    this->Display->printBuffer();
    });

  this->Network->Server81.onUpdateEvent(std::bind(&CL_App::setTime, this, _1, _2, _3, _4, _5, _6, _7));
  this->Network->Server81.onStartDisplayTimers([this](bool i){ if(i){this->setDisplayTimer();}else{this->stopDisplayTimer();}});
  this->Network->Server81.onLightsOff([this](){ this->Display->lightsDown(); });
  this->Network->Server81.onReload(std::bind(&CL_App::reLoadHandler, this));
  this->Network->Server81.onConfig([this](uint8_t i, bool write){ 
    if(write){this->Config->save(i);}else{this->Config->load(i);}
    });
  
  this->Network->onSetLed(std::bind(&CL_App::setLed, this, _1, _2));
  this->Network->onShowProgress([this](uint8_t i){ this->Display->showProgress(i); });
  //this->Network->onSetLed([this](int i, int color){this->setLed(i, color);});

  this->Term.onConnectWifi([this](const char* u,const char* p){ this->Network->connect(u,p);delay(5);});
  this->Term.onDateShow([this](){this->i2c.i2c_rtc_serialprint();});

  this->Cron = new CL_Cron();
  this->Cron->onRunCmd([this](String m){
    Serial.println("Cmd from cron:");
    Serial.println(m);
    this->Network->Server81.internalMessage(m);
    });
  

  i2c.begin();
  
  this->Display = new CL_Display();
  this->Display->begin();

//  for(uint8_t i = 0; i<100; i++ ){
//    this->Display->showProgress(i);
//    delay(20);
//  }
//  delay(2000);
  
  this->Network->begin();
  // сеть будет какое-то время подниматься
  this->Display->reloadColors();



  setDisplayTimer();
  
//  _TickerSelf.attach_ms_scheduled(5,std::bind(&CL_App::loop, this));
_Ticker1000.attach_ms_scheduled( 1000 ,std::bind(&CL_App::scheduled1000, this));

}

void CL_App::reLoadHandler(){
  this->Display->reloadColors();
  if(this->_switchDelay != this->Display->_switchDelay * 100){
    this->setSwitchTimer();
    }
  }

void CL_App::setDisplayTimer(){
  scheduledHourMin();
  setSwitchTimer();
  _Ticker500.attach_ms_scheduled(500,std::bind(&CL_App::scheduled500, this));
  this->Display->setTimers();
  }

void CL_App::setSwitchTimer(){
  this->_switchDelay = this->Display->_switchDelay * 100;
  _TickerHM.attach_ms_scheduled( _switchDelay ,std::bind(&CL_App::scheduledHourMin, this)); // 5000  
  }

void CL_App::stopDisplayTimer(){
  _Ticker500.detach();
  _TickerHM.detach();
  this->Display->stopTimers();
  }

void CL_App::loop(){
  this->Term.loop();
  this->Network->loop();
  }
uint8_t value = 0;

void CL_App::scheduled500(){
  this->shareWSnow();
  }

void CL_App::scheduled1000(){
    if(this->i2c.getSeconds() == 0){
      this->Cron->setNow(
        this->i2c.getMinutes(),
        this->i2c.getHours(),
        this->i2c.rtc->day(),
        this->i2c.rtc->month(),
        this->i2c.rtc->dayOfWeek()
      );
    }
}

void CL_App::scheduledHourMin(){
    this->showTimeWords();
//  this->showNumberTest();
    //this->Display->showTimeWords(41,true);
  }

void CL_App::showNumberTest(){
  this->Display->showTimeWords(++numb,0);
  }
void CL_App::showTimeWords(){
    uint8_t h = i2c.getHours();
    uint8_t m = i2c.getMinutes();
    uint8_t value = (ss) ? m : h;
    this->Display->showTimeWords(value,(ss)?2:1,st);
    ss = !ss; if(!ss) st = !st;  
  }


void CL_App::demo(){
  for(int i=0; i<110*1.5; i++){
    if(i < 110) this->Display->leds[i] = CRGB::White;
    for(int r = i-1; r>=0; r--){
      if(r < 110) this->Display->leds[r].subtractFromRGB(5);
      }
    FastLED.show();
    delay(50);
    }
  }

void CL_App::setTime(int s, int i, int h, int dOw, int d, int m, int y){
    if(!this->i2c.i2c_rtc_addr) return;
    Serial.printf("CL_App::setTime %d-%d-%d &d:%d:%d [%d]\n\r",y,m,d,h,i,s,dOw);
    this->i2c.rtc->set(s,i,h,dOw,d,m,y);
    this->i2c.i2c_rtc_serialprint();
  }

void CL_App::setLed(int i, int color){
  /*
  Serial.print("CN_App::ShowNumber ");
  Serial.print(i);Serial.print("=>");
  Serial.print(d);Serial.print(" ");
  Serial.println(color,HEX);
  */
  this->Display->leds[i] = color;
  FastLED.show();
}

void CL_App::shareWSnow(){
  if(this->i2c.i2c_rtc_addr){
    this->Network->Server81.now.dayOfWeek = this->i2c.rtc->dayOfWeek();
    this->Network->Server81.now.year = this->i2c.rtc->year();
    this->Network->Server81.now.month = this->i2c.rtc->month();
    this->Network->Server81.now.dayOfMonth = this->i2c.rtc->day();
    this->Network->Server81.now.minute = this->i2c.getMinutes();
    this->Network->Server81.now.hour = this->i2c.getHours();
    this->Network->Server81.now.second = this->i2c.getSeconds();
//    this->Network->Server81.now.mode = displayMode;
//    this->Network->Server81.timer = timerSeconds;
    }
  }
