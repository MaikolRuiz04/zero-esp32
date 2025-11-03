#include "ui/timer_display.h"
#include "ui/taskbar.h" // for Mode and currentMode
#include "ui_design.h"
#include <Adafruit_GFX.h>

// Internal state for optimized redraw
static int lastMin = -1;
static int lastSec = -1;

void resetTimerDisplayState() {
  lastMin = -1;
  lastSec = -1;
}

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
  uint16_t timerColor = COLOR_TIMER_TEXT;
  tft.setTextColor(timerColor, ILI9341_BLACK);
  tft.setTextSize(textSize);
  tft.setCursor(x, y);
  tft.print(buf);
}

void drawTimerOptimized(Adafruit_ILI9341 &tft, int minutes, int seconds) {
  int textSize = FONT_SIZE_TIMER;
  int charW = 6 * textSize; // base Adafruit 5 wide +1 spacing
  int charH = 8 * textSize;
  int totalChars = 5; // MM:SS
  int textWidth = totalChars * charW;
  int slotY = SLOT_Y;
  int slotH = SLOT_H;
  int top = slotY + slotH;
  int bottom = tft.height();
  int x0 = (tft.width() - textWidth) / 2;
  int y0 = top + ((bottom - top) - charH) / 2;
  uint16_t timerColor = COLOR_TIMER_TEXT;
  tft.setTextSize(textSize);
  tft.setTextColor(timerColor, ILI9341_BLACK);

  // Convert to digits
  int mT = minutes / 10; int mO = minutes % 10;
  int sT = seconds / 10; int sO = seconds % 10;
  int last_mT = (lastMin < 0) ? -1 : lastMin / 10;
  int last_mO = (lastMin < 0) ? -1 : lastMin % 10;
  int last_sT = (lastSec < 0) ? -1 : lastSec / 10;
  int last_sO = (lastSec < 0) ? -1 : lastSec % 10;

  // Helper lambda to clear+print a single character
  auto drawCharAt = [&](int index, char c) {
    int cx = x0 + index * charW;
    tft.fillRect(cx, y0, charW, charH, ILI9341_BLACK);
    tft.setCursor(cx, y0);
    tft.print(c);
  };

  // Force full redraw if first time
  if (lastMin < 0 || lastSec < 0) {
    char buf[6];
    sprintf(buf, "%02d:%02d", minutes, seconds);
    for (int i = 0; i < 5; ++i) drawCharAt(i, buf[i]);
    lastMin = minutes; lastSec = seconds; return;
  }

  // Minutes tens (index 0)
  if (mT != last_mT) drawCharAt(0, '0' + mT);
  // Minutes ones (index 1)
  if (mO != last_mO) drawCharAt(1, '0' + mO);
  // Colon (index 2) - colon never changes; could skip unless styling differs
  // Seconds tens (index 3)
  if (sT != last_sT) drawCharAt(3, '0' + sT);
  // Seconds ones (index 4)
  if (sO != last_sO) drawCharAt(4, '0' + sO);
  // If colon was never drawn (first incremental redraw without full), ensure it's present
  if (lastMin < 0 || lastSec < 0) drawCharAt(2, ':');
  else if (last_mT != mT || last_mO != mO || last_sT != sT || last_sO != sO) {
    // colon stays; but if aesthetic flicker is observed we could reprint colon once per minute change
  }
  lastMin = minutes;
  lastSec = seconds;
}
