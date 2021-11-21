#define LED_PIN     D3
#define NUM_LEDS    11*10
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define WL_MAX 11

#include <FastLED.h>
#include <EEPROM.h>
#include "Ticker.h"

class CL_Display{
  public:
    //CL_Display(){}
    void setup(void);
    void loop(void);
    void begin(void);
    void lightsUp(uint8_t *,uint8_t);
    void lightsDown();
    void type(uint8_t);
    void _type(void);
    void _clear(bool);
    bool showTimeWords(uint8_t d, uint8_t unit_t = 0, bool st=false); /* unit: 0 = none, 1 = hourm 2 = minute */
    void showProgress(uint8_t);
    void printBuffer(void);

    void setTimers(void);
    void stopTimers(void);
    
    CRGB leds[NUM_LEDS];
    CRGB main_color = CRGB(250,250,250);//CRGB::White;
    CHSV main_chsv = CHSV(250,250,250);
    bool sd[NUM_LEDS]; // shift direction
    bool sd_def = false;
    uint8_t br[NUM_LEDS];
    CRGB matrix[11][10];
    int delta_v[NUM_LEDS];

    uint8_t* up;
    Ticker _TickerFrame;
    Ticker _TickerCandle;
    Ticker _TickerPrint;

    uint8_t _delayFrame = 20;
    uint8_t _delayCandle = 15;
    uint8_t _downdCandle = 64;
    uint8_t _stepdCandle = 1;
    uint8_t _switchDelay = 50;
    
    uint8_t _delayPrint = 64;
    uint8_t config;

    void Candle(void);
    void CandleHSV(void);

    uint8_t print_buffer[32];
    uint8_t print_buffer_size = 0;
    uint8_t shift_buffer[32];
    uint8_t shift_buffer_size = 0;
    

    bool _frameReady=false;
    void frameReady(void);
    void showFastLED(void);
    void reloadColors(void);
    
    /* Слова */
    
    uint8_t u00[21][12] = {
      {9, 255, 255, 255, 255, 255}, // пусто
      {9,  10, 29,   30, 255, 255}, // один
      {89, 90, 109, 255, 255, 255}, // два
      {87, 92, 107, 255, 255, 255}, // три
      {27, 32,   6,  13,  53,  66}, // четыре
      {49, 50,  69,  70, 255, 255}, // пять
      {28, 31,  48,  51,  68, 255}, // шесть
      {47, 52,  67,  72, 255, 255}, // семь
      {7,  12,  47,  52,  67,  72}, // 8
      {8,  11,  71,  88,  91, 108}, // 9
      {8,  11,  73,  86,  93, 106},  // 10
      {9,  10,  29,  30,      5, 14, 25, 34, 45, 54, 65 }, // 11
      {26, 33,  46,           5, 14, 25, 34, 45, 54, 65 }, // 12
      {87, 92, 107,           5, 14, 25, 34, 45, 54, 65 }, // 13
      {27, 32,   6,  13,  53, 5, 14, 25, 34, 45, 54, 65 }, // 14
      {49, 50,  69,           5, 14, 25, 34, 45, 54, 65 }, // 15
      {28, 31,  48,  51,      5, 14, 25, 34, 45, 54, 65 }, // 16
      {47, 52,  67,           5, 14, 25, 34, 45, 54, 65 }, // 17
      {7,  12,  47, 52,  67,  5, 14, 25, 34, 45, 54, 65 }, // 18
      {8,  11,  71, 88,  91,  5, 14, 25, 34, 45, 54, 65 }, // 19
      {26, 33,  14, 25,  34, 45, 54, 65}, // 20
    };
    uint8_t u10[10][10] = {
      {}, // 0
      {}, // 10
      {36, 43,  56, 63,  77, 82, 97,102}, // 20
      { 3, 16,  23, 36,  77, 82, 97,102}, // 30
      {64, 75,  84, 95, 104}, // 40
//      {0,  19,  20, 39,  21, 38, 41, 58, 61}, // 50
      {76, 83,  96,103,  57, 62, 78, 81, 98}, // 50
      {28, 31,  48, 51,  68, 57, 62, 78, 81, 98}, // 60
      };
    uint8_t y00[6][4]{
      {}, // 0
      {35,36,37,42}, // одна
      {26,33,46},     // две
      { 3,16,23}, // 3 (внизу)
      {}, // 4
      {0,19,20,39}, // пять
      };
    uint8_t y10[6][10]{
      {}, // 0
      {}, // 10
      {26, 33,  14, 25,  34, 45, 54, 65}, // 20 вверху
      {87, 92, 107, 25,  34, 45, 54, 65}, // 30 вверху
      {64, 75,  84, 95, 104}, // 40
      {49, 50,  69,  70, 255, 8, 11, 73, 86, 93}, // 50
      };
    uint8_t hl[3][5]{
      {4,15,24,35,44}, // часов
      {74,85,94},      // час
      {74,85,94,105},  // часа
      };
    uint8_t ml[3][6]{
      {40,59,60,79,80},    // минут
      {40,59,60,79,80,100},// минута
      {40,59,60,79,80,99}, // минуты
      };

    /* Матрица */
//    CRGB matrix[11][10];

  };
