#include "CL_Wire.h"

void CL_Wire::begin(){
  Wire.begin(DS3231_SDA, DS3231_SCL);
  i2c_scan();
  if(i2c_rtc_addr){
    this->rtc = new uRTCLib(i2c_rtc_addr,URTCLIB_MODEL_DS3231);//,URTCLIB_MODEL_DS3231
    //this-> rtc->set(0, 42, 16, 6, 2, 5, 15);
    _Ticker.attach_ms_scheduled(500,std::bind(&CL_Wire::i2c_rtc_update, this));
    }else{
      Serial.println("No RTC device found =(");
    }
  }

void CL_Wire::loop(){
  if(i2c_rtc_addr){
    /*if(millis()%500==0){
      this->rtc->refresh();
      }*/
    if(millis()%1000 == 0){
      //  i2c_rtc_serialprint();
      }
    }else{
      esp_ntp_log();
      }
  }

void CL_Wire::i2c_scan(){
  
  uint8_t error, address;
  int nDevices;

  Serial.println("Scanning i2c...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);Serial.println("  =)");
      i2c_found(address);
      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Wire error at address 0x");
      if (address<16) Serial.print("0");
      //Serial.print(address,HEX);
      //Serial.print(": ");Serial.print(error);
      Serial.println();
      
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  }

void CL_Wire::i2c_found(uint8_t address){ // bool if true, stop.
  if((address == 0x68) && !i2c_rtc_addr){
    switch(address){
      case 0x68:Serial.println("0x68 is RTC");break;
      case 0x57:Serial.println("0x57 is DS3231");break;
      }
    i2c_rtc_addr = address;
    }
  }

void CL_Wire::i2c_rtc_serialprint(){
  if(!i2c_rtc_addr) return;
  this->rtc->refresh();
  Serial.print("RTC DateTime: ");
  Serial.print(rtc->year());
  Serial.print('/');
  Serial.print(rtc->month());
  Serial.print('/');
  Serial.print(rtc->day());

  Serial.print(' ');

  Serial.print(rtc->hour());
  Serial.print(':');
  Serial.print(rtc->minute());
  Serial.print(':');
  Serial.print(rtc->second());

  Serial.print(" DOW: ");
  Serial.print(rtc->dayOfWeek());

  Serial.print(" - Temp: ");
  Serial.print(rtc->temp()  / 100);

  Serial.println();
  }

void CL_Wire::i2c_rtc_update(){
  if(!i2c_rtc_addr) return;
  this->rtc->refresh();
  }

void CL_Wire::i2c_rtc_settime(int s, int i, int h, int dOw, int d, int m, int y){
   if(!i2c_rtc_addr) return;
//   Serial.println("i2c_rtc_settime");
   /*
   void uRTCLib::set(
      const uint8_t second, 
      const uint8_t minute, 
      const uint8_t hour, 
      const uint8_t dayOfWeek, 
      const uint8_t dayOfMonth, 
      const uint8_t month, 
      const uint8_t year
    )
   */
   this->rtc->set(s, i, h, dOw, d, m, y);
   //i2c_rtc_serialprint();
  }

void CL_Wire::esp_ntp_start(long offset){
//      DateTime.setServer("pool.ntp.org");
//      DateTime.setTimeZone(offset/3600);
        DateTime.begin();
//      if (!DateTime.isTimeValid()) {
//        Serial.println("ESPDateTime: Failed to get time from server.");
//      }  
      //esp_ntp_log();
  }

void CL_Wire::esp_ntp_log(){
  Serial.println(DateTime.now());
  Serial.println(DateTime.getTime());
  Serial.println(DateTime.utcTime());
  Serial.println("--------------------");
  Serial.println(DateTime.toString());
  Serial.println(DateTime.toISOString());
  Serial.println(DateTime.toUTCString());
  Serial.println("--------------------");
  Serial.println(DateTime.format(DateFormatter::COMPAT));
  Serial.println(DateTime.format(DateFormatter::DATE_ONLY));
  Serial.println(DateTime.format(DateFormatter::TIME_ONLY));
  Serial.println("--------------------");
  DateTimeParts p = DateTime.getParts();
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d %ld %+05d\n", p.getYear(),
                p.getMonth(), p.getMonthDay(), p.getHours(), p.getMinutes(),
                p.getSeconds(), p.getTime(), p.getTimeZone());
  Serial.println("--------------------");
  time_t t = DateTime.now();
  Serial.println(DateFormatter::format("%Y/%m/%d %H:%M:%S", t));
  Serial.println(DateFormatter::format("%x - %I:%M %p", t));
  Serial.println(DateFormatter::format("Now it's %F %I:%M%p.", t));
  }


int CL_Wire::getHours(){
  if(i2c_rtc_addr){return this->rtc->hour();}else{
    if(!DateTime.isTimeValid()) DateTime.forceUpdate();
    if(DateTime.isTimeValid()){
      DateTimeParts p = DateTime.getParts();
      return p.getHours();
      }
    }
  return 0;
  }
int CL_Wire::getMinutes(){
  if(i2c_rtc_addr){return this->rtc->minute();}else{
    if(!DateTime.isTimeValid()) DateTime.forceUpdate();
    if(DateTime.isTimeValid()){
      DateTimeParts p = DateTime.getParts();
      return p.getMinutes();
      }
    }
  return 0;
  }
int CL_Wire::getSeconds(){
  if(i2c_rtc_addr){return this->rtc->second();}else{
    if(!DateTime.isTimeValid()) DateTime.forceUpdate();
    if(DateTime.isTimeValid()){
      DateTimeParts p = DateTime.getParts();
      return p.getSeconds();
      }
    }
  return 0;
  }
