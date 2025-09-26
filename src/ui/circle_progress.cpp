#include "ui/taskbar.h" // for Mode and currentMode
#include "ui_design.h"
#include "ui/circle_progress.h"
#include <Adafruit_GFX.h>
#include <math.h>

// Forward helper to convert degrees to radians
static inline float deg2rad(float d){ return d * 3.14159f / 180.0f; }

// Static state shared between calls (for baseline/full clearing logic)
static float g_lastFullDrawPercent = -1.0f; // mirrors internal lastPercent in drawCircleProgress

void resetCircleProgressState() {
  g_lastFullDrawPercent = -1.0f;
}

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
  int thickness = PROGRESS_THICKNESS;
  // Only clear the arc area, not the whole circle/background
  float startAngle = -90; // top
  if (percent < 0) percent = 0; if (percent > 1) percent = 1;
  static float lastPercent = -1.0f; // track to know when to clear entire ring
  if (g_lastFullDrawPercent < 0 && lastPercent >= 0) {
    // External reset requested
    lastPercent = -1.0f;
  }
  float endAngle = startAngle + 360 * percent;
  // Erase previous arc by overdrawing with background color (soft pastel)
  extern Mode currentMode;
  uint16_t arcBgColor = (currentMode == FOCUS) ? COLOR_ACCENT_OCHRE : COLOR_SLOT;
  float angleStep = PROGRESS_ANGLE_STEP; // configurable smoothness
  // If decreasing, clear full ring
  if (lastPercent > percent || lastPercent < 0) {
    for (float a = startAngle; a < startAngle + 360.0f; a += angleStep) {
      float rad = a * 3.14159f / 180.0f;
      // Draw thickness as a short radial span (reduces gaps)
      for (int t = 0; t < thickness; t++) {
        int x0 = cx + (r - t) * cos(rad);
        int y0 = cy + (r - t) * sin(rad);
        tft.drawPixel(x0, y0, arcBgColor);
      }
    }
  }
  // Draw only new segment if increasing
  float fromAngle = (lastPercent < 0 || lastPercent > percent) ? startAngle : (startAngle + 360.0f * lastPercent);
  if (fromAngle < startAngle) fromAngle = startAngle;
  for (float a = fromAngle; a < endAngle; a += angleStep) {
    float rad = a * 3.14159f / 180.0f;
    // Instead of individual pixels, draw a short radial line for solidity
    int outerX = cx + r * cos(rad);
    int outerY = cy + r * sin(rad);
    int innerX = cx + (r - thickness + 1) * cos(rad);
    int innerY = cy + (r - thickness + 1) * sin(rad);
    tft.drawLine(innerX, innerY, outerX, outerY, color);
  }
  lastPercent = percent;
  g_lastFullDrawPercent = lastPercent;
}

void drawCircleProgressDelta(Adafruit_ILI9341 &tft, float lastPercent, float newPercent, uint16_t color, int diameter) {
  if (newPercent < 0) newPercent = 0; if (newPercent > 1) newPercent = 1;
  if (lastPercent < 0) lastPercent = 0; if (lastPercent > 1) lastPercent = 1;
  if (newPercent <= lastPercent) return; // nothing to add
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
  int thickness = PROGRESS_THICKNESS;
  float startAngle = -90.0f + 360.0f * lastPercent;
  float endAngle = -90.0f + 360.0f * newPercent;
  float delta = endAngle - startAngle;
  if (delta <= 0.0f) return;
  // Ensure we always draw something even if delta < PROGRESS_ANGLE_STEP
  float angleStep = PROGRESS_ANGLE_STEP;
  if (delta < angleStep) {
    angleStep = delta / 2.0f; // draw at least start & end (will include end with <=)
    if (angleStep < 0.05f) angleStep = delta; // extremely tiny; single sweep
  }
  // Use per-pixel radial shell fill for solidity (prevents sparse gaps between angle steps)
  for (float a = startAngle; a <= endAngle + 0.0001f; a += angleStep) {
    float rad = deg2rad(a);
    for (int t = 0; t < thickness; t++) {
      int x = cx + (r - t) * cos(rad);
      int y = cy + (r - t) * sin(rad);
      tft.drawPixel(x, y, color);
    }
  }
  // Touch-up: explicitly draw endAngle to guarantee closure
  float radEnd = deg2rad(endAngle);
  for (int t = 0; t < thickness; t++) {
    int x = cx + (r - t) * cos(radEnd);
    int y = cy + (r - t) * sin(radEnd);
    tft.drawPixel(x, y, color);
  }
}
