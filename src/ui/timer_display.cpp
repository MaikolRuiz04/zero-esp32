#include "ui/timer_display.h"
#include <Adafruit_GFX.h>

void drawTimer(Adafruit_ILI9341 &tft, int minutes, int seconds) {
  char buf[6];
  sprintf(buf, "%02d:%02d", minutes, seconds);
  int textSize = 5;
  int textWidth = 5 * 6 * textSize;
  int textHeight = 8 * textSize;
  int taskbarHeight = 30;
  int usableHeight = tft.height() - taskbarHeight;
  int x = (tft.width() - textWidth) / 2;
  int y = taskbarHeight + (usableHeight - textHeight) / 2;
  tft.fillRect(x, y, textWidth, textHeight, ILI9341_BLACK); // clear with black background
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(textSize);
  tft.setCursor(x, y);
  tft.print(buf);
}
