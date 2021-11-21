#include "CL_Term.h"
CL_Term::CL_Term(){}
void CL_Term::loop(){

    if(Serial.available()){
    String cmd = Serial.readStringUntil('\n\r');
    String args[4];
    int argl = 0;
    if(cmd){
      Serial.println("> "+cmd);


      ////
      //  Распилим команду на кусочки, разделяя пробелом!
      ////
        
      int sep = cmd.indexOf(" ");
      while(sep > -1){
        args[argl] = cmd.substring(0,sep); argl++;
        cmd = cmd.substring(sep+1);
        sep = cmd.indexOf(" ");
        }
      if(sep == -1) args[argl] = cmd; argl++;
      //if(argl == 1) args[0  ] = cmd; argl++;

      //Serial.printf("args[%d]=",argl); for(int i=0;i<argl;i++) Serial.print(args[i]+","); Serial.println();
      
      if(cmd=="reboot"){
        Serial.println("Perform to reboot");
        ESP.restart();
        return;
        }
  
      if(cmd=="help"){
        Serial.println("ls [dir]");
        Serial.println("reboot");
        Serial.println("uptime");
        Serial.println("iwconfig");
        Serial.println("iwconfig scan");
        Serial.println("iwconfig connect SSID Pass");
        return;
        }

      if(cmd=="uptime"){
        Serial.print("Uptime: ");
        unsigned long s = millis()/1000;
        unsigned int d = s/86400;
        if(d > 0){ Serial.print(d);Serial.print(" day");if(d%10 > 1)Serial.print("s"); }
        unsigned int ds = s % 86400;
        Serial.print(ds/3600);Serial.print(":");
        Serial.print(ds%3600/60);Serial.print(":");
        Serial.print(ds%3600%60);
        Serial.println();
        return;
        }

      if(args[0] == "ls"){
        if(!args[1]) args[1] = "/";
        Serial.println("FS List:");
        Dir dir = LittleFS.openDir(args[1]);
          while (dir.next()) {
            File entry = dir.openFile("r");
            Serial.println(entry.name());
            }
        return;
        }

      /* iwconfig
       * */
      if(args[0] == "iwconfig"){
        // info
        if(argl == 1){
          WiFi.printDiag(Serial);
          Serial.print("localIPaddr: ");Serial.println(WiFi.localIP());
          Serial.println("Definitions: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h");
          return;
        }
        // scan
        if(args[1] == "scan"){
          delay(100);
          //String enc[8];enc[2]="TKIP (WPA)";enc[4]="CCMP (WPA)";enc[5]="WEP";enc[7]="NONE";enc[8]="AUTO";
          Serial.println("** Scan Networks **");
          uint8_t n = WiFi.scanNetworks();
          if(n > 0){
            Serial.print("Networks found:");Serial.println(n);
            for (int i = 0; i < n; ++i){
              String m = " * " + WiFi.SSID(i)+" ("+WiFi.RSSI(i)+") MAC: "+WiFi.BSSIDstr(i);//+" Enc: "+enc[WiFi.encryptionType(i)];
              Serial.println(m);
              delay(10);
              }
          Serial.println("** Scan Networks done **");
          }else{
            Serial.println("No Networks Found");
          }
         return; 
        }

        if(args[1] == "connect"){
          if(argl != 4){ Serial.println("three words please"); return;}
          //Serial.print("So, Let's try to connect wifi: ");Serial.print(args[1]);Serial.print(" with password: ");Serial.print(args[2]);Serial.println();
          return runConnectWifi(args[2],args[3]);
          }

        if(args[1] == "remove" && args[2]){
         Serial.println("WS iwconfig connect");
         String cf = "/connection_"; cf+= args[2]; cf+= ".txt";
         if(LittleFS.exists(cf)) LittleFS.remove(cf);
         return; 
        }

        if(args[1] == "disconnect" && args[2]){
          WiFi.disconnect();
          return; 
          }


        return;
        }
      /* date and time
       * */

      if(args[0] == "date"){
        // just show
        if(args[1] == ""){
          runDateShow();
          return; 
          }
        }

      /* isOnline
       * */

      if(args[0] == "isOnline"){
        WiFiClient client;
        HTTPClient http;
        http.begin(client, "http://gstatic.com/generate_204");
        int httpCode = http.GET();
        Serial.print("isOnline: ");
        Serial.println((httpCode==204)?"yes":"no");
        return;
        // 
        }
        
      Serial.println("help to list commands");
        
      }
    }
}
