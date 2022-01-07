# Word Clock! // Словесные часы (буквенные)

This is yet another version of **[Qlocktwo](https://qlocktwo.com/)** built on ESP8266 MCU.
Demo page and docs: **[Демо страница](https://timophey.github.io/qlocktwo/)**

## Libraries for build backend // Библиотеки для сборки прошивки
 - [ESP8266 Arduino Core](https://github.com/esp8266/Arduino) (2.7.4)
	 - ESP8266WiFi
	 - ESP8266WebServer
	 - ESP8266HTTPClient
	 - ESP8266HTTPUpdateServer
	 - ESP8266mDNS
	 - DNSServer
	 - EEPROM
	 - Wire
	 - Ticker
	 - LittleFS
	 - ArduinoOTA
 - [ESPDateTime](https://github.com/mcxiaoke/ESPDateTime)
 - [FastLED](https://github.com/FastLED/FastLED)
 - [uRTCLib](https://github.com/Naguissa/uRTCLib)
 - [NTPClient](https://github.com/arduino-libraries/NTPClient)
 - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
 - [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets/)
 - esp8266FTPServer - forked for LittleFS support (will be later)

## Useful tools // пригодятся инструменты
 - [LittleFS Filesystem Uploader](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin)
 - [EspExceptionDecoder](https://github.com/me-no-dev/EspExceptionDecoder)


## Connect

```
╔══════════════╗     ╔══════════╗
║              D1 -> SCL DS3231 ║
║ ESP 8266 MCU D2 -> SDA    RTC ║
║              ║     ╚══════════╝
║              D3 -> WS2812 LED ║
╚══════════════╝     ╚══════════╝
```
### WS2812 LED strip
```
 ╔═╗ ╔═╗ ╔═╗ ╔═╗ ╔═╗ ╔═ NC
 О Д И Н П Я Т Ь Д В А 
 Д Е Ш Е С Т Ь В Я Т Ь 
 В О Ч Е С Е М Ь Т Р И 
 Т Ы Д В А Р Е С Я Т Ь 
 Н А Д Ц А Т Ь Ч А С А 
 Ч А С О В Д С О Р О К 
 Т Р И Д В А Д П Я Т Ь 
 П Я Т Н А Д Е Ц А Т Ь 
 А М Д Е С Я Т С Я Т Ь 
 П Я Т Ь М И Н У Т Ы А 
 ║ ╚═╝ ╚═╝ ╚═╝ ╚═╝ ╚═╝ 
 ╚═ MCU
```
 
