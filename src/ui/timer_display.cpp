#include "ui/timer_display.h"
#include "ui/taskbar.h" // for Mode and currentMode
#include "ui_design.h"
#include <Adafruit_GFX.h>

void drawTimer(Adafruit_ILI9341 &tft, int minutes, int seconds) {
  char buf[6];
  sprintf(buf, "%02d:%02d", minutes, seconds);
  int textSize = FONT_SIZE_TIMER;
  int textWidth = 5 * 6 * textSize;
  int textHeight = 8 * textSize;
  // Center between slot bottom and screen bottom
  int slotY = 8, slotH = 36;
  int top = slotY + slotH;
  int bottom = tft.height();
  int x = (tft.width() - textWidth) / 2;
  int y = top + ((bottom - top) - textHeight) / 2;
  tft.fillRect(x, y, textWidth, textHeight, ILI9341_BLACK); // clear with black background
  extern Mode currentMode;
  uint16_t timerColor = 0xFFFF; // White for both modes
  tft.setTextColor(timerColor, ILI9341_BLACK);
  tft.setTextSize(textSize);
  tft.setCursor(x, y);
  tft.print(buf);
}
