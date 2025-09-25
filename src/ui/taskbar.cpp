#include "ui/taskbar.h"
#include <Adafruit_GFX.h>

void drawTaskbar(Adafruit_ILI9341 &tft, Mode mode) {
  tft.fillRect(0, 0, tft.width(), 30, 0x7BEF); // gray
  tft.setTextColor(ILI9341_WHITE, 0x7BEF);
  tft.setTextSize(2);
  // Draw FOCUS
  const char* focusText = "FOCUS";
  int focusLen = 5;
  int charWidth = 6; // Adafruit GFX default
  int textSize = 2;
  int focusPixelWidth = focusLen * charWidth * textSize;
  int focusX = 20;
  int focusY = 8;
  tft.setCursor(focusX, focusY);
  tft.print(focusText);
  // Draw BREAK
  const char* breakText = "BREAK";
  int breakLen = 5;
  int breakPixelWidth = breakLen * charWidth * textSize;
  int breakX = tft.width() - breakPixelWidth - 20;
  int breakY = 8;
  tft.setCursor(breakX, breakY);
  tft.print(breakText);
  // Draw underline below active mode
  int underlineY = 28;
  int underlineWidth, underlineX;
  if (mode == FOCUS) {
    underlineWidth = focusPixelWidth;
    underlineX = focusX;
  } else {
    underlineWidth = breakPixelWidth;
    underlineX = breakX;
  }
  tft.drawLine(underlineX, underlineY, underlineX + underlineWidth, underlineY, ILI9341_WHITE);
}
