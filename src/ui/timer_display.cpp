#include "ui/timer_display.h"
#include "ui/taskbar.h" // for Mode and currentMode
#include "ui_design.h"



#include <TFT_eSPI.h>

// Internal state for optimized redraw

static int lastMin = -1;
static int lastSec = -1;

void resetTimerDisplayState() {
  lastMin = -1;
  lastSec = -1;
}


void drawTimer(TFT_eSPI &tft, int minutes, int seconds) {
  // Only update if changed
  if (minutes == lastMin && seconds == lastSec) return;

  int slotY = SLOT_Y;
  int slotH = SLOT_H;
  int top = slotY + slotH;
  int bottom = tft.height();
  int centerX = tft.width() / 2;
  // Center timer between bottom of slot and bottom of screen
  int timerAreaTop = top;
  int timerAreaBottom = bottom;
  int timerAreaH = timerAreaBottom - timerAreaTop;
  int centerY = timerAreaTop + timerAreaH / 2;

  tft.setTextFont(7); // Use 7-segment, square font
  int charW = 36, charH = 48; // Font 7: charH is 48px (approx)
  int maxW = tft.width() - 10; // 5px offset per side
  int maxH = timerAreaH - 10;
  // Treat as 4 chars + colon, both sides tight
  float digitCharW = 1.7f * charW; // Each 2 digits (MM or SS) is 1.7x charW wide
  float colonCharW = 0.6f * charW; // Colon is narrower
  float totalCharW = (2 * digitCharW) + colonCharW;
  // Maximize textSize by height first, then width
  int textSizeH = maxH / charH;
  int textSizeW = maxW / (int)totalCharW;
  int textSize = (textSizeH < textSizeW) ? textSizeH : textSizeW;
  if (textSize < 1) textSize = 1;
  tft.setTextSize(textSize);

  // Calculate positions for each part
  int totalW = (int)(totalCharW * textSize);
  int xStart = centerX - totalW / 2;
  int yStart = centerY - (charH * textSize) / 2;

  // Draw minutes if changed
  if (minutes != lastMin) {
    char minBuf[3];
    sprintf(minBuf, "%02d", minutes);
    // Erase old
    tft.fillRect(xStart, yStart, (int)(digitCharW * textSize), charH * textSize, TFT_BLACK);
    // Draw new
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(minBuf, xStart, yStart);
  }
  // Always draw colon (centered, less gap)
  int colonX = xStart + (int)(digitCharW * textSize);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(":", colonX, yStart);
  // Draw seconds if changed
  if (seconds != lastSec) {
    char secBuf[3];
    sprintf(secBuf, "%02d", seconds);
    int secX = colonX + (int)(colonCharW * textSize);
    // Erase old
    tft.fillRect(secX, yStart, (int)(digitCharW * textSize), charH * textSize, TFT_BLACK);
    // Draw new
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(secBuf, secX, yStart);
  }

  lastMin = minutes;
  lastSec = seconds;

  tft.setTextFont(1); // Reset to default
  tft.setTextSize(1);
}


// drawTimerOptimized is now obsolete; use drawTimer for flicker-free, artifact-free updates
