#include "countdown_clock.h"
#include <Arduino.h>

void showClock(Adafruit_ILI9341 &tft) {
  int minutes = 25;
  int seconds = 0;
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(4);
  tft.setCursor(60, 100);

  for (int total = 1500; total >= 0; total--) {
    minutes = total / 60;
    seconds = total % 60;
    tft.fillRect(60, 100, 200, 50, ILI9341_BLACK); // Clear previous
    tft.setCursor(60, 100);
    char buf[6];
    sprintf(buf, "%02d:%02d", minutes, seconds);
    tft.print(buf);
    delay(1000);
  }
}
