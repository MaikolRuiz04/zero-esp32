#include "ui/taskbar.h" // for Mode and currentMode
#include "ui_design.h"
#include "ui/circle_progress.h"
#include <Adafruit_GFX.h>
#include <math.h>

void drawCircleProgress(Adafruit_ILI9341 &tft, float percent, uint16_t color, int diameter) {
  // Center between slot bottom and screen bottom
  int top = SLOT_Y + SLOT_H;
  int bottom = tft.height();
  int cx = tft.width() / 2;
  int cy = top + (bottom - top) / 2;
  int availableHeight = bottom - top;
  int r;
  if (diameter > 0) {
    r = diameter / 2;
  } else {
    r = (tft.width() < availableHeight ? tft.width() : availableHeight) / 2 - 20;
  }
  int thickness = 8;
  // Only clear the arc area, not the whole circle/background
  float startAngle = -90; // top
  float endAngle = startAngle + 360 * percent;
  // Erase previous arc by overdrawing with background color (soft pastel)
  extern Mode currentMode;
  uint16_t arcBgColor = (currentMode == FOCUS) ? COLOR_ACCENT_OCHRE : COLOR_SLOT;
  for (float a = startAngle; a < startAngle + 360; a += 0.5) {
    float rad = a * 3.14159 / 180.0;
    for (int t = 0; t < thickness; t++) {
      int x = cx + (r - t) * cos(rad);
      int y = cy + (r - t) * sin(rad);
      tft.drawPixel(x, y, arcBgColor);
    }
  }
  // Draw solid arc for progress (smoother)
  for (float a = startAngle; a < endAngle; a += 0.5) {
    float rad = a * 3.14159 / 180.0;
    for (int t = 0; t < thickness; t++) {
      int x = cx + (r - t) * cos(rad);
      int y = cy + (r - t) * sin(rad);
      tft.drawPixel(x, y, color);
    }
  }
}
