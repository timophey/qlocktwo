//#include <stdint.h>
#include "CN_WS.h"

#include <WebSocketsServer.h>

CN_WS::CN_WS(){}

using namespace std::placeholders;
void CN_WS::begin(){
  /* https://github.com/Links2004/arduinoWebSockets/issues/14#issuecomment-298737197 */
  this->wss = new WebSocketsServer(81);
  this->wss->onEvent(std::bind(&CN_WS::onWsEvent, this, _1, _2, _3, _4));
  this->wss->begin();
  Serial.println("WS  server started");
  }

void CN_WS::loop(){
  this->wss->loop();
  }

void CN_WS::onWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){
    String cmd = "";for(int i=0; i < length; i++){cmd+=(char)payload[i];}
    this->onWsEventStr(num, type, cmd);
    }
  }
  
void CN_WS::onWsEventStr(uint8_t num, WStype_t type, String cmd){
  if (type == WStype_TEXT){

//   cmd
    
    // JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, cmd);

    if(doc["cmd"] == "getEEPROM"){
      DynamicJsonDocument odoc(2048);
      odoc["cmd"] = "eeprom";
      int addr_from = 0; if(doc["from"]) addr_from=doc["from"].as<int>();
      int addr_to = 256; if(doc["to"]) addr_to=doc["to"].as<int>();
      
      for(int addr=addr_from; addr<addr_to+1; addr++){
        odoc["data"][String(addr)] = EEPROM.read(addr);
        /*if(addr>=144 && addr<=226){
          odoc["data"][String(addr)] = word(EEPROM.read(addr), EEPROM.read(addr+1));
          addr++;
          }*/
        //Serial.print(addr);
        //Serial.print(' ');
        //Serial.println(EEPROM.read(addr));
        }
      String response;
      serializeJson(odoc, response);
      sendString(response,num);
      }
    
    if(doc["cmd"]=="commit"){Serial.println("Perform to commit");EEPROM.commit();commited = true;}

    if(doc["cmd"]=="reboot"){Serial.println("Perform to reboot");ESP.restart();}

    if(cmd == "now" || doc["cmd"]=="getNOW"){this->sendNow(num);return;}

    if(doc["cmd"] == "setTime"){
      Serial.println("WS: setTime");
      if(doc["now"]["s"] && doc["now"]["i"] && doc["now"]["h"] && doc["now"]["w"] && doc["now"]["d"] && doc["now"]["m"] && doc["now"]["y"] || true){
        Serial.printf("runUpdateEvent(%d,%d,%d,%d,%d,%d,%d)\n\r",
          doc["now"]["s"].as<int>(), 
          doc["now"]["i"].as<int>(), 
          doc["now"]["h"].as<int>(), 
          doc["now"]["w"].as<int>(), 
          doc["now"]["d"].as<int>(), 
          doc["now"]["m"].as<int>(), 
          doc["now"]["y"].as<int>());
        runUpdateEvent(
          doc["now"]["s"].as<int>(), 
          doc["now"]["i"].as<int>(), 
          doc["now"]["h"].as<int>(), 
          doc["now"]["w"].as<int>(), 
          doc["now"]["d"].as<int>(), 
          doc["now"]["m"].as<int>(), 
          doc["now"]["y"].as<int>()
          );
        }
      }
    
    if(doc["cmd"] == "setTimer"){
      //Serial.print("setTimer");Serial.println(doc["s"].as<int>());
      if(doc["s"]){
        runSetTimer(doc["s"].as<int>());
        }
      }

    if(doc["cmd"] == "setHsv"){
        EEPROM.write(0x0D,doc["value"]["h"].as<int>());
        EEPROM.write(0x0E,doc["value"]["s"].as<int>());
        EEPROM.write(0x0F,doc["value"]["v"].as<int>());
        commited = false;
//        RunReloadFunction();
    }

    if(doc["cmd"] == "setColor"){
        int r = doc["value"]["r"].as<int>();
        int g = doc["value"]["g"].as<int>();
        int b = doc["value"]["b"].as<int>();
        EEPROM.write(0x10,r);
        EEPROM.write(0x11,g);
        EEPROM.write(0x12,b);
        commited = false;
        RunReloadFunction();
    }

    if(doc["page"]=="colors" && doc["cmd"] == "assign"){

        uint8_t csp[3] = {0xA0, 0xC0, 0xE0}; // // color start points
        // parse
        int d = doc["d"].as<int>();
        int j = doc["j"].as<int>();
        int r = doc["value"]["r"].as<int>();
        int g = doc["value"]["g"].as<int>();
        int b = doc["value"]["b"].as<int>();
        //int a = 0xA0 + d*3;
        // prevent too large values
        if(d>6 || r>255 || g>255 || b>255 || j>2) return;
        int icsp = csp[j];
        int a = icsp + d*3;
        // digits
        int n[7]={now.hour/10,now.hour%10,now.minute/10,now.minute%10,now.second/10,now.second%10,now.second%8};
        
        // put into EEPROM
        EEPROM.write(a+0,r);
        EEPROM.write(a+1,g);
        EEPROM.write(a+2,b);

        // show now
        uint32_t c = 0;
          c |= r << 16;
          c |= g << 8;
          c |= b << 0;
        // 24 bits whitten
        //runShowNumber(d,n[d],c);
        RunReloadFunction();
      }

    if(doc["cmd"] == "setEEPROM"){
      bool ok = true;
      int addr = doc["addr"].as<int>();
      uint8_t val = doc["value"].as<int>();
//      if(doc["addr"] >= 128 && doc["addr"] <= 240) ok=true; // 160 - 240
      if(ok){
        EEPROM.write(addr,val);
        commited = false;
      }else{
        Serial.print("setEEPROM ");
        Serial.print(addr);
        Serial.print("is bad idea");
        }
      }

    if(doc["cmd"] == "reloadColors"){
      RunReloadFunction();
      }

    if(doc["cmd"] == "setDisplayMode" && doc["value"]){
      runSetDisplayMode(doc["value"].as<int>());
      }

    if(doc["cmd"] == "setConfig" && doc["value"].as<int>() > -1){//  
      runConfig(doc["value"].as<int>(), doc["write"].as<bool>() );
      }

    if(doc["cmd"] == "startDisplayTimers" && doc["value"].as<int>() > -1){
      Serial.printf("WS: startDisplayTimers %d\n\r",doc["value"].as<int>());
      runStartDisplayTimers((doc["value"].as<int>()==1));
      }

    if(doc["cmd"] == "lightsOff"){
      Serial.println("WS: lightsOff");
      runLightsOff();
      }
    if(doc["cmd"] == "setPixel" && doc["x"] > -1 && doc["y"] > -1 && doc["value"] > -1){ // 
      /* this.sendWS({cmd:"setPixel",x:x,y:y,value:0xFFFFFF}); */
      runSetPixel(doc["x"].as<int>(),doc["y"].as<int>(),doc["value"].as<int>());
//      Serial.printf("WS: setPixel %d,%d,%d \n\r",doc["x"].as<int>(),doc["x"].as<int>(),doc["value"].as<int>());
      }
    

    /* WiFi
     * */ 
    if(doc["cmd"] == "iwconfig" && doc["args"]){
      DynamicJsonDocument odoc(1024);
      odoc["cmd"]=doc["cmd"];
      
      // switch variants
      if(doc["args"][0] == "scan"){
        Serial.println("WS iwconfig scan start");
        int n = WiFi.scanNetworks();
        Serial.println("WS iwconfig scan done");
        if (n > 0) {
          for (int i = 0; i < n; i++) {
            //String iS = String(i);
            String cf = "/connection_"; cf+= WiFi.SSID(i); cf+= ".txt";
            odoc["network"][i]["SSID"] = WiFi.SSID(i);
            odoc["network"][i]["encT"] = WiFi.encryptionType(i);
            odoc["network"][i]["RSSI"] = WiFi.RSSI(i);
            odoc["network"][i]["conf"] = LittleFS.exists(cf);
          }
        }
      }

      if(doc["args"][0] == "connect" && doc["args"][1] && doc["args"][2]){
        Serial.println("WS iwconfig connect");
        runConnectWifi(doc["args"][1],doc["args"][2]);
        }

      if(doc["args"][0] == "disconnect"){
        Serial.println("WS iwconfig disconnect");
        WiFi.disconnect();
        }

      if(doc["args"][0] == "remove" && doc["args"][1]){
         String cf = "/wifi_connection_" + doc["args"][1].as<String>() + ".txt";
         Serial.println("WS iwconfig remove "+cf);
         if(LittleFS.exists(cf)){
          LittleFS.remove(cf);
          Serial.println("WS iwconfig " + doc["args"][1].as<String>() + " removed from FS");
         }else{
          Serial.println("File "+cf+" not exists");
          }
          WiFi.disconnect();
          delay(500);
          runAutoconnect();
        }
      
      // end of scan
      // current state data
      odoc["ifconfig"]["RSSI"]  = WiFi.RSSI();
      odoc["ifconfig"]["SSID"]  = WiFi.SSID();
      odoc["ifconfig"]["status"]  = WiFi.status() * 1;
      odoc["ifconfig"]["hostname"]  = WiFi.hostname();
      odoc["ifconfig"]["localIP"]  = WiFi.localIP().toString();
      odoc["ifconfig"]["softAPIP"] = WiFi.softAPIP().toString();
      odoc["ifconfig"]["subnetMask"] = WiFi.subnetMask().toString();
      odoc["ifconfig"]["gatewayIP"] = WiFi.gatewayIP().toString();
      odoc["ifconfig"]["dnsIP1"] = WiFi.dnsIP(0).toString();
      odoc["ifconfig"]["dnsIP2"] = WiFi.dnsIP(1).toString();
      // return all
      String response;
      serializeJson(odoc, response);
      sendString(response,num);
    }
    // end of wifi

    if(doc["cmd"] == "updatentp"){
      Serial.println("WS: updatentp");
      runUpdateNTP();
      }
  }
}

void CN_WS::sendNow(uint8_t num = 0){//
//Serial.println(num);
      DynamicJsonDocument doc(512);
      //if(displaymode != 0x12) getRTC();
      doc["cmd"] = "now";
      doc["now"]["h"] = now.hour;
      doc["now"]["i"] = now.minute;
      doc["now"]["s"] = now.second;
      doc["now"]["y"] = now.year;
      doc["now"]["m"] = now.month;
      doc["now"]["d"] = now.dayOfMonth;
      doc["now"]["w"] = now.dayOfWeek;
      //doc["now"]["t"] = now.temp;
      doc["timer"] = timer;
      doc["mode"] = now.mode;
      doc["displayValues"]["0"] = displayValues[0];
      doc["displayValues"]["1"] = displayValues[1];
      doc["displayValues"]["2"] = displayValues[2];
      // RT dynamic values
      doc["EEPROM"]["1024"] = 0;
      doc["commited"] = commited;
      /*
      
      doc["EEPROM"]["218"] = EEPROM.read(218);
      doc["EEPROM"]["219"] = EEPROM.read(219);
      doc["EEPROM"]["220"] = EEPROM.read(220);
      doc["EEPROM"]["221"] = EEPROM.read(221);
      doc["EEPROM"]["222"] = EEPROM.read(222);
      doc["EEPROM"]["223"] = EEPROM.read(223);

      doc["EEPROM"]["225"] = EEPROM.read(225);
      doc["EEPROM"]["226"] = EEPROM.read(226);
      doc["EEPROM"]["227"] = EEPROM.read(227);
      doc["EEPROM"]["228"] = EEPROM.read(228);
      doc["EEPROM"]["229"] = EEPROM.read(229);
      doc["EEPROM"]["230"] = EEPROM.read(230);
      */
      
      
      String response;
      serializeJson(doc, response);
      sendString(response,num);
      //webSocketSendTimerStatus(num);
}

void CN_WS::sendString(String response, uint8_t num){
        int response_len = response.length() + 1;
        char response_wsFrame[response_len];
        response.toCharArray(response_wsFrame, response_len);
        //Serial.println(response_wsFrame);
        //return response_wsFrame;
        if(num > 0){
          wss->sendTXT(num,response_wsFrame);  
          }else{
          wss->broadcastTXT(response_wsFrame,response_len-1);  
          }
  }

/*void CN_WS::runUpdateEvent(int s, int i, int h, int dOw, int d, int m, int y){
  
  }*/
