#include "features/countdown_clock.h"
#include <Arduino.h>

void showClock(Adafruit_ILI9341 &tft) {
  int minutes = 25;
  int seconds = 0;
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  int textSize = 5; // slightly bigger than before
  tft.setTextSize(textSize);

  for (int total = 1500; total >= 0; total--) {
    minutes = total / 60;
    seconds = total % 60;
    char buf[6];
    sprintf(buf, "%02d:%02d", minutes, seconds);
    int textWidth = 5 * 6 * textSize; // 5 chars, 6 pixels per char, times text size
    int textHeight = 8 * textSize; // 8 pixels high per char
    int x = (tft.width() - textWidth) / 2;
    int y = (tft.height() - textHeight) / 2;
    tft.fillRect(x, y, textWidth, textHeight, ILI9341_BLACK); // Clear previous
    tft.setCursor(x, y);
    tft.print(buf);
    delay(1000);
  }
}
