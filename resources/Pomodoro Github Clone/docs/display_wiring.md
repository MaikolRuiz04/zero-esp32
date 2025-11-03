# ESP32 and Waveshare 4.26" E-Paper Display Setup

> [!NOTE]
> Special thanks to [@dibolei](https://github.com/dibolei) for contributing this documentation.

## Parts used for this Documentation

- Waveshare 4.26-inch E-Paper Display HAT, 800x480 (SSD1677 controller)
- ESP32 (ESP-WROOM-32 Development Board)

## Pinout and Wiring

On newer revisions of these display boards, there is a PWR pin that needs to be pulled HIGH in software or tied to the 3.3v line. If this pin does not have power, the board will not try to draw from VCC and therefore not be powered.

NOTE: The wiring below is based on the ESP-WROOM-32 Development Board (sometimes also called ESP32S).
As such, your wiring may vary slightly.

| Display Pin | ESP32 GPIO |
|-------------|------------|
| VCC         | 3.3V       |
| GND         | GND        |
| DIN (MOSI)  | GPIO 23    |
| CLK (SCK)   | GPIO 18    |
| CS          | GPIO 5     |
| DC          | GPIO 17    |
| RST         | GPIO 16    |
| BUSY        | GPIO 4     |
| PWR         | GPIO 22 (Set HIGH in "main.cpp" setup() function, around line 86 and 87 as of the time of this writing) |

## Basic Test Sketch

Below is a basic Arduino sketch using the GxEPD2 library to test the display functionality.

```cpp
#include <GxEPD2_BW.h>

#define EPD_CS    5
#define EPD_DC    17
#define EPD_RST   16
#define EPD_BUSY  4
#define EPD_PWR   22

GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT> 
  display(GxEPD2_426_GDEQ0426T82(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(EPD_PWR, OUTPUT);
  digitalWrite(EPD_PWR, HIGH);
  delay(100);

  display.init(115200, true, 2, false);
  display.setRotation(1);

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(5);
    display.setCursor(200, 240);
    display.print("Hello, World!");
  } while (display.nextPage());

  delay(2000);

  int partialY = 360;
  int partialH = 40;
  display.setPartialWindow(0, partialY, display.width(), partialH);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(3);
    display.setCursor(10, partialY + 24);
    display.print("Partial refresh demo");
  } while (display.nextPage());

  delay(2000);
  display.hibernate();
}

void loop() { }
```
