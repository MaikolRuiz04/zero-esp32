

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "countdown_clock.h"

#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22
#define TFT_SCK  18
#define TFT_MISO 19
#define TFT_MOSI 23

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  tft.begin();
  tft.setRotation(1);
  showClock(tft);
}

void loop() {}
