#include "ui/taskbar.h" // for Mode and currentMode
#include "ui_design.h"
#include "ui/circle_progress.h"

#include <TFT_eSPI.h>
#include <math.h>
#include "core/timer_controller.h"
#include "ui/theme_provider.h"

// Forward helper to convert degrees to radians
static inline float deg2rad(float d){ return d * 3.14159f / 180.0f; }

// Static state shared between calls (for baseline/full clearing logic)
static float g_lastFullDrawPercent = -1.0f; // mirrors internal lastPercent in drawCircleProgress
static unsigned long g_cycleStartMs = 0;
static int g_cycleTotalMs = 0;
static float g_continuousLastPercent = -1.0f;
static float g_lastDrawnAngle = -1000.0f; // absolute angle in degrees of last drawn progress (start at sentinel)

void resetCircleProgressState() {
  g_lastFullDrawPercent = -1.0f;
  g_continuousLastPercent = -1.0f;
  g_lastDrawnAngle = -1000.0f;
}

#if CONTINUOUS_PROGRESS
void startProgressCycle(int totalSeconds) {
  g_cycleStartMs = millis();
  g_cycleTotalMs = totalSeconds * 1000;
  g_continuousLastPercent = -1.0f;
  g_lastFullDrawPercent = -1.0f;
  g_lastDrawnAngle = -90.0f; // logical start (top)
}

static float computeContinuousPercent() {
  if (g_cycleTotalMs <= 0) return 0.0f;
  unsigned long now = millis();
  unsigned long elapsed = (now >= g_cycleStartMs) ? (now - g_cycleStartMs) : 0UL;
  if (elapsed >= (unsigned long)g_cycleTotalMs) return 1.0f;
  return (float)elapsed / (float)g_cycleTotalMs;
}

void updateProgressAnimation(TFT_eSPI &tft, uint16_t /*focusColor*/, uint16_t /*breakColor*/, int diameter) {
  extern Mode currentMode;
  float targetPercent = computeContinuousPercent();
  if (targetPercent < 0) targetPercent = 0; if (targetPercent > 1) targetPercent = 1;
  // Use ThemeProvider for progress color
  TimerMode tmode = (currentMode == FOCUS) ? TimerMode::Focus : TimerMode::Break;
  TimerController dummyController(1500, 300); // dummy values, only mode matters
  dummyController.reset(tmode);
  uint16_t color = ThemeProvider::progressColor(dummyController);
  // Geometry (duplicate logic for center/r)
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

  float targetAngle = -90.0f + targetPercent * 360.0f;
  if (g_lastDrawnAngle < -900.0f) {
    // first frame: draw initial span from start to target
    g_lastDrawnAngle = -90.0f;
  }
  // Angle step for roughly one pixel arc length: deg_per_pixel = 57.2958 / r
  float pixelStep = 57.2957795f / (float)r;
  if (pixelStep < 0.2f) pixelStep = 0.2f; // clamp lower bound to avoid huge loops on large radius
  // Draw forward until we reach targetAngle
  int stepsThisCall = 0;
  const int MAX_STEPS_PER_CALL = 40; // safety to avoid long blocking frame if behind
  while (g_lastDrawnAngle + pixelStep <= targetAngle && stepsThisCall < MAX_STEPS_PER_CALL) {
    float a = g_lastDrawnAngle + pixelStep;
    float rad = a * 3.14159f / 180.0f;
    float cosv = cos(rad);
    float sinv = sin(rad);
    for (int t = 0; t < thickness; ++t) {
      int rr = r - t;
      int x = cx + (int)(rr * cosv);
      int y = cy + (int)(rr * sinv);
      tft.drawPixel(x, y, color);
    }
    g_lastDrawnAngle = a;
    stepsThisCall++;
  }
  g_continuousLastPercent = targetPercent;
}
#endif

void drawCircleProgress(TFT_eSPI &tft, float percent, uint16_t color, int diameter) {
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
  // Erase previous arc by overdrawing with background color (slot fill color)
  extern Mode currentMode;
  TimerMode tmode = (currentMode == FOCUS) ? TimerMode::Focus : TimerMode::Break;
  TimerController dummyController(1500, 300); // dummy values, only mode matters
  dummyController.reset(tmode);
  uint16_t arcBgColor = ThemeProvider::slotFillColor(dummyController);
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

void drawCircleProgressDelta(TFT_eSPI &tft, float lastPercent, float newPercent, uint16_t color, int diameter) {
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
