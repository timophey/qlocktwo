//#include "CN_Wifi.h"
#include "CN_WebServer.h"

CN_WebServer::CN_WebServer(){}

using namespace std::placeholders;

void CN_WebServer::setup(){
  // INIT
  uint8_t g = EEPROM.read(0x02);
  bool editor_enabled = (bitRead(g, 4));
  bool upgrade_enabled = (bitRead(g, 5));
  bool captive_enabled = (bitRead(g, 3));
       cache_enabled = (bitRead(g, 2));
       stand_mode = (bitRead(g, 7));
  this->httpd = new ESP8266WebServer(80);
/*
  ESP8266HTTPUpdateServer httpUpdater;
  httpUpdater.setup(&this->httpd);
  */
  // root
  this->httpd->on("/", std::bind(&CN_WebServer::handleRoot, this)); // , _1, _2, _3, _4
    // list
  this->httpd->on("/list", std::bind(&CN_WebServer::handleFileList, this));
  this->httpd->on("/ifconfig.json", std::bind(&CN_WebServer::handleWiFiStatus, this));
  this->httpd->on("/sysinfo.json", std::bind(&CN_WebServer::handleSysInfo, this));
  this->httpd->on("/locale.js", HTTP_GET, [this](){
    // read locale from EEPROM
    uint8_t loc = EEPROM.read(148); 
    char hexString[2]; itoa(loc, hexString, 16);
    char path[13];
    sprintf(path,"/locale_%s.js",hexString);//  path+= loc + ".js";
    if(!handleFileRead(path)){
      this->httpd->send(404, "text/plain", "FileNotFound ");
      };
    });

  /* Editor
   * */
  if(editor_enabled){
    Serial.println("httpd: register Editor");
        //load editor
        this->httpd->on("/edit", HTTP_GET, [this]() {
           if (!handleFileRead("/edit.htm")) {
            this->httpd->send(404, "text/plain", "FileNotFound /edit.htm");
          }
        });
        //create file
        this->httpd->on("/edit", HTTP_PUT, std::bind(&CN_WebServer::handleFileCreate, this));
        //delete file
        this->httpd->on("/edit", HTTP_DELETE, std::bind(&CN_WebServer::handleFileDelete, this));
      
        //first callback is called after the request has ended with all parsed arguments
        //second callback handles file uploads at that location
        this->httpd->on("/edit", HTTP_POST, 
          [this]() {this->httpd->send(200, "text/plain", "");}, 
          std::bind(&CN_WebServer::handleFileUpload, this)
        );
        
        // upload
        this->httpd->on("/upload", HTTP_POST,                           // if the client posts to the upload page
          [this](){ this->httpd->send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
          std::bind(&CN_WebServer::handleFileUpload, this)                                    // Receive and save the file
        );
      }
      
  // reboot
  this->httpd->on("/reboot", [this](){
    Serial.println("Perform to reboot");
    this->httpd->send(505, "text/html", "Reboot...<br><a href='/'>Home</a><script>location='/'</script>");
    delay(500);
    ESP.restart();
    });

  // uptime -> returns sec after startup
  
  this->httpd->on("/uptime", [this](){
    this->httpd->sendHeader("Access-Control-Allow-Origin", "*");
    this->httpd->send(200, "text/plain", String(millis()/1000));
    });
    

  // get file
  this->httpd->onNotFound([this]() {                              // If the client requests any URI
    if (!handleFileRead(this->httpd->uri()))                  // send it if it exists
      this->httpd->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  /* Firmware Upgrade
   * */
  if(upgrade_enabled){
    Serial.println("httpd: register Firmware Upgrade");
    //firmware
    this->httpd->on("/firmware", HTTP_GET,[this](){ 
      if (!handleFileRead("/firmware.html"))  
      this->httpd->send(404, "text/plain", "FileNotFound /firmware.html"); 
      });
    // upload
    this->httpd->on("/firmware", HTTP_POST, 
      [this]() {
        this->httpd->sendHeader("Connection", "close");
        this->httpd->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        delay(50);
        ESP.restart();
      }, 
      std::bind(&CN_WebServer::handleFirmwareUpdate, this) 
    ); // handleFirmwareUpdate
  }
  /* Captive Portal 
   * */
  if(captive_enabled){
    Serial.println("httpd: register Captive Portal");
    this->httpd->on("/hotspot-detect.html", std::bind(&CN_WebServer::handleRoot, this)); // Apple captive portal. Maybe not needed. 
    this->httpd->on("/generate_204", std::bind(&CN_WebServer::handleRoot, this)); // Android captive portal. Maybe not needed. Might be handled by notFound handler.
    this->httpd->on("/gen_204", std::bind(&CN_WebServer::handleRoot, this));  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
    this->httpd->on("/fwlink", std::bind(&CN_WebServer::handleRoot, this));  // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    this->httpd->on("/ncsi.txt", std::bind(&CN_WebServer::handleRoot, this));// Microsoft NCSI
    this->httpd->on("/chat", std::bind(&CN_WebServer::handleRoot, this));
  }
  
  this->httpd->on("/wifi_connections.json", std::bind(&CN_WebServer::handleWifiConnections, this));
//  this->httpd->on("/configs.json", std::bind(&CN_WebServer::handleConfigs, this));
  
}

void CN_WebServer::begin(){
  
  CN_WebServer::setup();

  Serial.println("* * *");
  httpd->begin();
  Serial.println("Web server started");

  // Updater
/*
  ESP8266WebServer httpServer(8080);
  ESP8266HTTPUpdateServer httpUpdater;
  httpUpdater.setup(&httpServer);
*/
  }

void CN_WebServer::loop(){
  httpd->handleClient();
  }

void CN_WebServer::handleRoot(){
  
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  
  Serial.println("handleRoot");
  if (!handleFileRead("/index.html")){
static const char serverIndex[] PROGMEM =
  R"(<!DOCTYPE html>
   <html lang='en'>
     <head>
         <meta charset='utf-8'>
         <meta name='viewport' content='width=device-width,initial-scale=1'/>
     </head>
     <body>
      <h1>Empty FS</h1>
      <h2>Upload File</h2>
      <form method='post' enctype='multipart/form-data' action='/upload'>
        <input type='file' name='name'><input type='submit' value='Upload FILE'>
       </form>
       <h2>Upload Flash Bin</h2>
       <form method='post' enctype='multipart/form-data' action='/firmware'>
        <input type='file' accept='.ino.bin,.ino.bin.gz' name='firmware'><input type='submit' value='Upload FLASH Bin'>
       </form>
       <h2>Upload FS Bin</h2>
       <form method='post' enctype='multipart/form-data' action='/firmware'>
        <input type='file' accept='.mklittlefs.bin,.mklittlefs.bin.gz' name='filesystem'><input type='submit' value='Upload LittleFS Bin'>
       </form>
     </body>
   </html>
  )";
    
    
    /*
    String res = "<h1>Upload edit.htm</h1><form method=\"post\" enctype=\"multipart/form-data\" action=\"/upload\"><input type=\"file\" name=\"name\"><input class=\"button\" type=\"submit\" value=\"Upload\"></form>";
    res += "<h1>Upload Flash Bin</h1><form method=\"post\" enctype=\"multipart/form-data\" action=\"/firmware\"><input type=\"file\" name=\"name\"><input class=\"button\" type=\"submit\" value=\"Upload\"></form>";
    res += "<h1>Upload LittleFS Bin</h1><form method=\"post\" enctype=\"multipart/form-data\" action=\"/firmware?fs=1\"><input type=\"file\" name=\"name\"><input type=hidden name=fs value=1><input class=\"button\" type=\"submit\" value=\"Upload\"></form>";
    */
    httpd->send(200, "text/html", serverIndex);
  }
}

//void CN_WebServer::handleFW(){
//  
//  }

void CN_WebServer::handleFileList() {
  if (!this->httpd->hasArg("dir")) {
    this->httpd->send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = this->httpd->arg("dir");
  String swf = this->httpd->arg("startsWith");
  
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = LittleFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");

    if(swf.length() && !String(entry.name()).startsWith(swf)) continue;
      
    if (output != "[") {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name());//.substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  this->httpd->sendHeader("Access-Control-Allow-Origin", "*");
  this->httpd->send(200, "text/json", output);
}

void CN_WebServer::handleFileCreate() {
  if (this->httpd->args() == 0) {
    return this->httpd->send(500, "text/plain", "BAD ARGS");
  }
  String path = this->httpd->arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/") {
    return this->httpd->send(500, "text/plain", "BAD PATH");
  }
  if (LittleFS.exists(path)) {
    return this->httpd->send(500, "text/plain", "FILE EXISTS");
  }
  File file = LittleFS.open(path, "w");
  if (file) {
    file.close();
  } else {
    return this->httpd->send(500, "text/plain", "CREATE FAILED");
  }
  this->httpd->send(200, "text/plain", "");
  path = String();
}

void CN_WebServer::handleFileDelete() {
  if (this->httpd->args() == 0) {
    return this->httpd->send(500, "text/plain", "BAD ARGS");
  }
  String path = this->httpd->arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/") {
    return this->httpd->send(500, "text/plain", "BAD PATH");
  }
  if (!LittleFS.exists(path)) {
    return this->httpd->send(404, "text/plain", "FileNotFound");
  }
  LittleFS.remove(path);
  this->httpd->send(200, "text/plain", "");
  path = String();
}

void CN_WebServer::handleFileUpload() {
  if (this->httpd->uri() != "/edit") {
    //return;
  }
  HTTPUpload& upload = this->httpd->upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = LittleFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void CN_WebServer::handleFirmwareUpdate(){
      HTTPUpload& upload = this->httpd->upload();

      // display vars
      int nx = 6;int values[nx];int prpcd = 0;

      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        String filename = upload.filename;
        
        if (upload.name == "filesystem") {
          if(filename.endsWith(".mklittlefs.bin")){
            size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
            Serial.printf("Update FS: %s\n", upload.filename.c_str());
            
            close_all_fs();
            WiFiUDP::stopAll();
            runStopAll();
            
            if (!Update.begin(fsSize, U_FS)){//start with max available size
              Update.printError(Serial);
            }
          }else{
            Serial.println("Wrong Filename");
            this->httpd->close();
          }
        }else
        if(upload.name == "firmware"){
            if(filename.endsWith(".ino.bin")){
              Serial.printf("Update FLASH: %s\n", upload.filename.c_str());
              uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
              
              WiFiUDP::stopAll();
              runStopAll();
  
              if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
                Update.printError(Serial);
              }
              
            }else{
            }
          }
        
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
        int prpc = prpcd = upload.totalSize / (upload.contentLength / 100);
        // display progress
        runShowProgress(prpcd);
        Serial.printf("Update Progress: %u%%\n\r", prpc);
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          prpcd = 100;
          // display progress
          runShowProgress(prpcd);
          Serial.printf("Update Success: %u%%\nRebooting...\n", 100);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }

      
      yield();
    }

void CN_WebServer::handleWiFiStatus(){
  DynamicJsonDocument doc(256);
  doc["RSSI"]  = WiFi.RSSI();
  doc["SSID"]  = WiFi.SSID();
  doc["status"]  = WiFi.status() * 1;
  doc["hostname"]  = WiFi.hostname();
  doc["localIP"]  = WiFi.localIP().toString();
  doc["softAPIP"] = WiFi.softAPIP().toString();
  doc["softAPSSID"] = WiFi.softAPSSID();
  doc["softAPPSK"] = WiFi.softAPPSK();
  doc["subnetMask"] = WiFi.subnetMask().toString();
  doc["gatewayIP"] = WiFi.gatewayIP().toString();
  doc["dnsIP1"] = WiFi.dnsIP(0).toString();
  doc["dnsIP2"] = WiFi.dnsIP(1).toString();
  String response;serializeJson(doc, response);
  this->httpd->send(200, "text/json", response);
  }

void CN_WebServer::handleSysInfo(){
  DynamicJsonDocument doc(1024);
  // LittleFS
  FSInfo fs_info;
  LittleFS.info(fs_info);
  char chipID[6];
  sprintf(chipID, "%06x", ESP.getChipId());
  doc["ESP"]["ChipId"] = chipID;
  doc["ESP"]["MaxFreeBlockSize"] = ESP.getMaxFreeBlockSize();
  doc["ESP"]["FreeHeap"] = ESP.getFreeHeap();
  doc["ESP"]["HeapFragmentation"] = ESP.getHeapFragmentation();
  doc["ESP"]["BootMode"] = ESP.getBootMode();
  doc["ESP"]["BootVersion"] = ESP.getBootVersion();
  doc["ESP"]["SdkVersion"] = ESP.getSdkVersion();
  doc["ESP"]["CoreVersion"] = ESP.getCoreVersion();
  doc["ESP"]["FullVersion"] = ESP.getFullVersion();
  doc["ESP"]["CpuFreqMHz"] = ESP.getCpuFreqMHz();
  doc["ESP"]["FlashChipSize"] = ESP.getFlashChipSize();
  doc["ESP"]["FlashChipSpeed"] = ESP.getFlashChipSpeed();
  doc["ESP"]["FlashChipMode"] = ESP.getFlashChipMode();
  doc["ESP"]["SketchSize"] = ESP.getSketchSize();
  doc["ESP"]["FreeSketchSpace"] = ESP.getFreeSketchSpace();
  doc["ESP"]["FullVersion"] = ESP.getFullVersion();

//  doc["FS"]["totaluint8_ts"] = fs_info.totaluint8_ts;
//  doc["FS"]["useduint8_ts"] = fs_info.useduint8_ts;
  doc["FS"]["blockSize"] = fs_info.blockSize;
  doc["FS"]["pageSize"] = fs_info.pageSize;
  doc["FS"]["maxOpenFiles"] = fs_info.maxOpenFiles;
  doc["FS"]["maxPathLength"] = fs_info.maxPathLength;
  doc["load"] = load;
  String response;serializeJson(doc, response);
  this->httpd->send(200, "text/json", response);  
  }

bool CN_WebServer::isOnline(){
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://gstatic.com/generate_204");
    int httpCode = http.GET();
    return (httpCode == 204);
  }
bool CN_WebServer::handleFileRead(String path) { // send the right file to the client (if it exists)
  if(cache_enabled) this->httpd->sendHeader("cache-control", "public, max-age=30672000");
  
  if(stand_mode && (path == "/index.html")){
    this->httpd->sendHeader("Location", "/color.html", true);
    this->httpd->send( 302, "text/plain", "Stand mode");
    return true;
    }
  /* CDN
   * */
//  if(this->httpd->hostHeader() != toStringIp(WiFi.softAPIP())){
//    String r302 = "";
//    
//    if(path == "/jquery-ui.min.js")            r302 = "https://code.jquery.com/ui/1.12.1/jquery-ui.min.js";
//    if(path == "/jquery-3.4.1.min.js")         r302 = "https://code.jquery.com/jquery-3.5.1.min.js";
//
    if(path == "/babel.min.js")                    // r302 = "https://cdnjs.cloudflare.com/ajax/libs/babel-standalone/6.26.0/babel.min.js";
        this->httpd->sendHeader("date", "Sat, 19 Sep 2020 20:24:55 GMT");
//    if(path == "/react-dom.production.min.js") r302 = "https://cdnjs.cloudflare.com/ajax/libs/react-dom/16.13.1/umd/react-dom.production.min.js";
//    if(path == "/react.production.min.js")     r302 = "https://cdnjs.cloudflare.com/ajax/libs/react/16.13.1/umd/react.production.min.js";
//
//    if(path == "/bootstrap.min.css")           r302 = "https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css";
//
//    if(r302.length() > 0 && isOnline()){
//      this->httpd->sendHeader("Location", r302, true);
//      this->httpd->send( 302, "text/plain", "");
//      return true;
//      }
//  }

  
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    this->httpd->sendHeader("Access-Control-Allow-Origin", "*");
    File file = LittleFS.open(path, "r");                    // Open the file
    if(file.isFile()){
      size_t sent = this->httpd->streamFile(file, contentType);    // Send it to the client
    }
    else{
      String output = "<h1>"+path+"</h1>";
      Dir dir= LittleFS.openDir(path);
      while( dir.next())
        {
          String fn = dir.fileName();
          output += "<li><a href='"+path+"/"+fn+"'>"+fn+"</a></li>";
        }
      this->httpd->send(200, "text/html", output);
      }
    
    file.close();                                          // Close the file again
    Serial.println(String("Sent file: ") + path);
    
    return true;
  }
  Serial.println(String("File Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

String CN_WebServer::getContentType(String filename){ // convert the file extension to the MIME type
  if (filename.endsWith(".htm") || filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js"))   return "application/javascript; charset=utf-8";
  else if (filename.endsWith(".gz"))   return "application/x-gzip";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  return "text/plain";
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean CN_WebServer::captivePortal() {
//  Serial.println(this->httpd->hostHeader());
//  Serial.println(String(myHostname) + ".local");
  if (!isIp(this->httpd->hostHeader()) && this->httpd->hostHeader() != (String(myHostname) + ".local")) {
    //Serial.println("Request redirected to captive portal");
    this->httpd->sendHeader("Location", String("http://") + toStringIp(this->httpd->client().localIP()), true);
    this->httpd->send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    this->httpd->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

boolean CN_WebServer::isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

String CN_WebServer::toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

void CN_WebServer::handleWifiConnections(){
  String output = "[";
  String fn;
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    File entry = dir.openFile("r");
    fn = entry.name();
//    fn = fn.substring(1);
    if(!fn.startsWith("wifi_connection_")) continue;
    if (output != "[") output += ','; // for not first
    String pwd; while (entry.available()) pwd += char(entry.read()); entry.close();
    output += "{\"SSID\":\"" + fn.substring(16,fn.length()-4) + "\",\"PASS\":\""+pwd+"\"}";
  }  
  output += "]";

  this->httpd->sendHeader("Access-Control-Allow-Origin", "*");
  this->httpd->send(200, "text/json", output);
}
