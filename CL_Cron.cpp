#include "CL_Cron.h"
/*

Crontab.json
  i:{
    event: [time|boot|connect],
    time: {i:_,h:_,d:_,m:_,w:_,} // минута час день месяц день_недели
    cmd: {cmd: 'command', arrg1: mixed, arrg2: mixed}
    
  }


*/

CL_Cron::CL_Cron(){}
  
void CL_Cron::setNow(uint8_t i, uint8_t h, uint8_t d, uint8_t m, uint8_t w){
//  this->trigger(4,this->w,w);this->w = w;
//  this->trigger(3,this->d,d);this->d = d;
//  this->trigger(2,this->m,m);this->m = m;
//  this->trigger(1,this->h,h);this->h = h;
//  this->trigger(0,this->i,i);this->i = i;
//  Serial.printf("CL_Cron::setNow(uint8_t %d, uint8_t %d, uint8_t %d, uint8_t %d, uint8_t %d) \n\r",w,d,m,h,i);
  uint8_t values_new[5] = {w,d,m,h,i};//{i,h,m,d,w};
  bool result = false;
  for(uint8_t j=0; j<5; j++){
    valuesNew[j] = values_new[j];
    if(valuesNew[j] != values[j]){
      if(!result && trigger()) result = true;
//      if(result) break;
      values[j] = values_new[j];
      }
    }


  }

bool CL_Cron::trigger(){
  // get config
  String fa = ".crontab.json";
  size_t size = 0;
  if (LittleFS.exists(fa)){
  File configFile = LittleFS.open(fa, "r");
    size = configFile.size();
    const int capacity = JSON_OBJECT_SIZE(3) + 2 * size;
    DynamicJsonDocument doc(capacity);
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DeserializationError error = deserializeJson(doc, buf.get());
    configFile.close();

    // foreach items
    if(doc["items"]){

      JsonArray arItems = doc["items"].as<JsonArray>();
      int is = arItems.size();
//      Serial.printf("itemsSize: %d\n\r",is);

      for (const JsonObject& obItem : arItems) {
        uint8_t p = 0;
        String cmd = obItem["cmd"];
//        Serial.println(cmd);
        // String k : tkeys
        for(uint8_t j=0; j<5; j++){
          String key = tkeys[j];
          String compare = obItem["time"][key];
          if(match(compare,valuesNew[j])){
//            Serial.printf("%2lu (%s) == %s \n\r", valuesNew[j], key.c_str(), compare.c_str());
            p++;
            }
          }

          if(p==5){
            // do magic
//            Serial.println("Bingo!");
            DynamicJsonDocument cdoc(128);
            cdoc["cmd"] = obItem["cmd"];
            // call cmd
            String response;
            serializeJson(cdoc, response);
            RunCmd(response);
            return true;
            }

        }
        
    }

    
//    config = configJson;
    }

    return false;
}

bool CL_Cron::match(String compare, uint8_t value){
  if(compare == "*" || compare == String(value)) return true;
  if(compare.startsWith("*/")){
    uint8_t co = compare.substring(2).toInt();
//    Serial.printf("CL_Cron::match del to %d \n\r",co);
    if(co > 0){
      if(value % co == 0) return true;
      }
    }
  return false;
  }
