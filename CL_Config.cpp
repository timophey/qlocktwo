#include "CL_Config.h"

void CL_Config::load(uint8_t index){

  // get filename and start vars
  char* fn = this->filename(index);
  File configFile = LittleFS.open(fn, "r"); if (!configFile){Serial.printf("Open file %s failed \n\r",fn);return;}
  size_t size = configFile.size(); 
  const int capacity = JSON_OBJECT_SIZE(3) + 2 * size;
  DynamicJsonDocument config_doc(capacity);
  
  // read file
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  
  // parse file
  DeserializationError error = deserializeJson(config_doc, buf.get());
  configFile.close();
  if(error){ Serial.printf("Failed to parse config file: %s\n",error.c_str());  return;}

  // apply to EEPROM
  if(config_doc["data"]){
    Serial.print("EEPROM set { ");
    for(uint8_t i = a0; i <= a1; i++){
      if(config_doc["data"][String(i)].as<String>().length()){
        uint8_t val = config_doc["data"][String(i)].as<int>();
        Serial.printf("i:%d, ",val);
        EEPROM.write(i,val);
        }
      }
    Serial.println("}");
    RunReloadFunction();
    EEPROM.write(216,index); // current Config loaded
    }

  }
  
void CL_Config::save(uint8_t index){
  char* fn = this->filename(index);
  String file_data;
  Serial.println(fn);
  
  // {"i":0,"data":{"160":255,     "1024":0}}
  file_data.concat("{\"i\":");
  file_data.concat(index);
  file_data.concat(",\"data\":{");
  for(uint8_t i = a0; i <= a1; i++){
    char tmp[7];sprintf(tmp, "\"%d\":%d,", i, EEPROM.read(i));
    file_data.concat(tmp);
  }
  file_data.concat("\"1024\":0}}");
  File f = LittleFS.open(fn, "w");
       f.print(file_data);
       f.close();
  Serial.println("CL_Config::save() OK");
  }

char* CL_Config::filename(uint8_t i){
//  String indexHex = String(i, HEX);
  char colors_config_tmp[16];
  sprintf(colors_config_tmp, "/.config_%x.json", i);
  return colors_config_tmp;
  }
