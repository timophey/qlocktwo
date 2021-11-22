#include "CL_Display.h";

void CL_Display::setup(){};
void CL_Display::loop(){};
void CL_Display::begin(){

  // init LED strip
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  // boot light
//  delay(500);
//  for(uint8_t i=0; i<NUM_LEDS; i++){
//    leds[i] = CRGB::White; FastLED.show();
//    delay(50);
//    leds[i] = CRGB::Black; FastLED.show();
//    }
//  delay(500);
  reloadColors();
  // off all and show must go on
  lightsDown();
//  setTimers(); /* call moved to reloadColors() */
  };

void CL_Display::setTimers(){
  _TickerFrame.attach_ms_scheduled(20, std::bind(&CL_Display::showFastLED, this));
  if(bitRead(config, 0)){
    _TickerCandle.attach_ms_scheduled(_delayCandle, std::bind(&CL_Display::CandleHSV, this));
    }else{
    _TickerCandle.attach_ms_scheduled(_delayCandle, std::bind(&CL_Display::Candle, this));    
    }
  uint8_t _pd = EEPROM.read(0x16); if(_pd == 0 || _pd == 255) _pd = 64;
  _TickerPrint.attach_ms_scheduled(_pd, std::bind(&CL_Display::_type, this));
  }
  
void CL_Display::stopTimers(){
  _TickerFrame.detach();
  _TickerCandle.detach();
  _TickerPrint.detach();
  }


bool CL_Display::showTimeWords(uint8_t d, uint8_t unit_t, bool st){
   bool is_min = (unit_t > 1);
//  Serial.printf("showTimeWords %d \n\r",d);
  this->lightsDown();

  uint8_t d10 = d / 10; // десятки
  uint8_t d0 = d % 10;  // единицы
  /* Десятки
   * */
  if(d > 20){
    
    if(is_min && (d % 10 == 1) && (y10[d10][0] > 0)){ // x1
//      Serial.printf("d10 = %d \n\r",d10);
      this->lightsUp(y10[d10],10);
    }else{
      if(is_min && (d10 == 2) && (d0 == 3 || d0 == 5 || d0 == 6  || d0 == 7)){ // 23,25,26,27 min
        this->lightsUp(u00[20],8);
      }else
      this->lightsUp(u10[d10],10);
    }
    if(d % 10) d = d % 10;
  }

  /* Единицы
   * */
  
  if(d <= 20){
    uint8_t d0 = d;// % 10;
    if(is_min && 
        (
          (d > 0) && (d < 3) 
          || (d10 == 5) && (d0==5) && st
          || (d10 == 2 || d10 == 4) && (d0==3)
        )
      ){
      this->lightsUp(y00[d0],4); // одна/две/ 55 / 43
    }else{
      if(is_min && d0 == 15 ){ // 15 min
          uint8_t up[10] = {2,17,22,37,42,57,77,82,97,102};
          this->lightsUp(up,10);
      }else
      if(is_min && (d10 == 2 || d10 == 4) && (d0 == 5)){ // 25,45 min
        uint8_t up[4] = {76,83,96,103};
        this->lightsUp(up,4);
        }else{
          this->lightsUp(u00[d0],12);
          }
      }
    }

  /*
  Часов, час, часа
  */
  if(unit_t == 2){
//    Serial.println("min");
    if(d==1) this->lightsUp(ml[1],6); 
    else if(d>=2 && d<=4) this->lightsUp(ml[2],6);
    else this->lightsUp(ml[0],5);
  }
  if(unit_t == 1){
//    Serial.println("hour");
    if(d==1) this->lightsUp(hl[1],3); 
    else if(d>=2 && d<=4) this->lightsUp(hl[2],4);
    else this->lightsUp(hl[0],5);
  }

  //print_buffer_shifed = print_buffer_size;
  return true;  
  }

void CL_Display::showProgress(uint8_t p){
  for(uint8_t i=0; i<NUM_LEDS; i++) leds[i] = CRGB::Black;
  for(uint8_t i=0; i<p; i++){
    uint8_t hue = (255 / 100) * (p-i);
    leds[i] = CHSV(hue,220,220-p);
  }
  FastLED.show();
  if(p == 100){
    int tt = 1024;
    for(uint8_t i=0; i<10; i++){
      delay(tt); tt/=2;
      leds[100+i] = CRGB(128,128,220 / 10 * (i+1));
      FastLED.show();
    }
  }
}

void CL_Display::lightsDown(){
  for(uint8_t i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Black;
    delta_v[i] = 0;
    sd[i] = sd_def;
    }
  FastLED.show();
  }

void CL_Display::lightsUp(uint8_t *ls,uint8_t len){
  //uint8_t len = (sizeof(ls)/sizeof(*ls));
  //Serial.printf("len: %d \n\r",len);
  for(uint8_t i = 0; i<len; i++){
    uint8_t l = ls[i];
    //Serial.println(l,DEC);
    if(l==255 || l==0 && i>0) continue;
    
    //leds[l] = CRGB::White;
    this->type(l);
    
    frameReady();
    //delay(25);
    }
  //FastLED.show();
  
  }

void CL_Display::Candle(){
//  int c0 = 1;
//  int c1 = 64;
  int c0 = _stepdCandle;
  int c1 = _downdCandle;
  if(!c0 || !c1) return;
  for(int i=0; i<NUM_LEDS; i++){
        CRGB rgb = leds[i];
//        CRGB mc = main_color;
        unsigned char rgbMin, rgbMax;
    
        rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
        rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);
        
        //Serial.printf("[%d] Candle %d %d %d\n\r",i,led.r,led.g,led.b);
//        if(rgbMin == 0) continue;
        if(!rgb.r && !rgb.g && !rgb.b) continue;
        
        if(rgb == main_color) sd[i] = false;

        if(
           (rgbMin < (main_color.r - c1) || (rgb.r - c0) < 0) && 
           (rgbMin < (main_color.g - c1) || (rgb.g - c0) < 0) && 
           (rgbMin < (main_color.b - c1) || (rgb.b - c0) < 0)
           ) sd[i] = true;//  || rgbMin < 1
        if(rgbMax > (main_color.r + c1) || rgbMax > (main_color.g + c1) || rgbMax > (main_color.b + c1) || rgbMax > 254) sd[i] = false;        
        
        if(sd[i] == false) leds[i].subtractFromRGB(c0);
        if(sd[i] == true) leds[i].addToRGB(c0);
        //Serial.printf("[%d] > %d \n\r",i,sd[i]);
        //leds[i] += (sd[i]) ? c0 : -c0;
        
        frameReady();
    }
  }

void CL_Display::CandleHSV(){
//  int c0 = _stepdCandle;
//  int c1 = _downdCandle;
  for(int i=0; i<NUM_LEDS; i++){
    CRGB rgb = leds[i];
    CHSV chsv = main_chsv;
    uint8_t rgbMin, rgbMax;
//    Serial.printf("rgbMin=%d \n\r",rgbMin);
    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);
    if(!rgbMax){
      delta_v[i] = 0;
      continue; // skip if black
      }
    // else make candle

    // bounce
    if(bitRead(config, 5)){
      if(sd[i]==true && (chsv.value + delta_v[i] + _stepdCandle) > 255){
        sd[i]=false;
//        Serial.printf("sd[i]==true, v=%d,dv=%d,s=%d \n\r",chsv.value,delta_v[i],_stepdCandle  );
        }
      if(sd[i]==false && (chsv.value + (delta_v[i] - _stepdCandle)) <=0 ){
        sd[i]=true;
//        Serial.printf("sd[i]==false, v=%d,dv=%d,s=%d \n\r",chsv.value,delta_v[i],_stepdCandle  );
        }

//      if(
//        (sd[i]==true && (chsv.value + delta_v[i] + _stepdCandle) >= 255)
//        ||
//        (sd[i]==false && (chsv.value - delta_v[i] - _stepdCandle) <= 0)
//        ) sd[i] = !sd[i];
      }else{
        // fall into 0
        if(bitRead(config, 3) && sd[i]==false){
    //      if(chsv.value - delta_v[i] == 0) continue;
          if(chsv.value - delta_v[i] - _stepdCandle <= 0) delta_v[i] = chsv.value - _stepdCandle;
          }
        
        // fall into 255
        if(bitRead(config, 4) && sd[i]==true){
          if(chsv.value + delta_v[i] + _stepdCandle == 255) continue;
          if(chsv.value + delta_v[i] + _stepdCandle >= 255) delta_v[i] = 255-(chsv.value+_stepdCandle);
          }
      }
    delta_v[i] += _stepdCandle * ((sd[i]) ? 1 : -1);
    chsv.value += delta_v[i];// (sd[i]) ? (chsv.value + delta_v[i]) : (chsv.value - delta_v[i]);

    hsv2rgb_rainbow( chsv, leds[i]);
//    hsv2rgb_spectrum( chsv, leds[i]);
    
//    Serial.printf("i=%d delta=%d \n\r",i,delta_v[i]);
      
    }
    frameReady();
  }

void CL_Display::type(uint8_t l){
  print_buffer[print_buffer_size] = l;  
  print_buffer_size++;
  }

void CL_Display::_type(){
  if(print_buffer_size == 0) return; // CRGB::White;//
  uint8_t l = print_buffer[0]; leds[l] = CRGB(main_color.r,main_color.g,main_color.b);
//  Serial.printf("P [%d] %d CRGB(%d,%d,%d)\n\r",print_buffer_size,l,main_color.red,main_color.green,main_color.blue);
  // shift main buffer
  //for(uint8_t i=0; i < print_buffer_size; i++) Serial.printf("%d,",print_buffer[i]); Serial.println();
  for(uint8_t i=0; i < print_buffer_size; i++) print_buffer[i] = print_buffer[i+1]; print_buffer_size--;
  
  // commit
  frameReady();
  }

void CL_Display::printBuffer(){
  while(print_buffer_size){_type();FastLED.show();delay(_delayFrame*2);}
}

void CL_Display::_clear(bool tail){
  
  }

void CL_Display::frameReady(){_frameReady=true;}
void CL_Display::showFastLED(){if(_frameReady){FastLED.show();_frameReady=false;/*delay(5);*/}}
void CL_Display::reloadColors(){
  uint8_t main_hue = EEPROM.read(0x0D);
  if(bitRead(config, 7)) main_hue = random(0,255);
  main_chsv = CHSV(main_hue,EEPROM.read(0x0E),EEPROM.read(0x0F));
  if(bitRead(config, 0)){
    hsv2rgb_rainbow( main_chsv, main_color);
    }else{
    main_color = CRGB(EEPROM.read(0x10),EEPROM.read(0x11),EEPROM.read(0x12));
    }
  
  
  config = EEPROM.read(0x03);
  Serial.printf("main_chsv = CHSV(%d,%d,%d)\n\r",main_chsv.h,main_chsv.s,main_chsv.v);
  Serial.printf("main_color = CRGB(%d,%d,%d)\n\r",main_color.r, main_color.g, main_color.b);
  _downdCandle = EEPROM.read(0x13);
  _stepdCandle = EEPROM.read(0x14);
  _switchDelay = EEPROM.read(0x15);
  _delayCandle = EEPROM.read(0x17);
  // начальное направление
  if(bitRead(config, 2)) sd_def = true;
  if(bitRead(config, 1)) sd_def = false;
  for(int i=0; i<NUM_LEDS; i++) sd[i]=sd_def;
  
  Serial.println(config,BIN);
  setTimers();
//  Serial.println("CL_Display::reloadColors");
  }