#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22
#define TFT_SCK  18
#define TFT_MISO 19
#define TFT_MOSI 23

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  // If it still stays black, try lowering speed:
  // SPI.setFrequency(16000000);

  tft.begin();
  tft.setRotation(1);

  tft.fillScreen(ILI9341_RED);   delay(300);
  tft.fillScreen(ILI9341_GREEN); delay(300);
  tft.fillScreen(ILI9341_BLUE);  delay(300);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(16,16);
  tft.println("Hello ILI9341!");
}

void loop() {}
