#pragma once
#include <Adafruit_ILI9341.h>
void drawCircleProgress(Adafruit_ILI9341 &tft, float percent, uint16_t color, int diameter = -1);
// Draw only the delta between lastPercent and new percent (smooth mode)
void drawCircleProgressDelta(Adafruit_ILI9341 &tft, float lastPercent, float newPercent, uint16_t color, int diameter = -1);
// Reset internal cached state so a subsequent draw with percent=0 repaints the full track
void resetCircleProgressState();

#if CONTINUOUS_PROGRESS
// Start a timed cycle (totalSeconds length). Call once per mode start.
void startProgressCycle(int totalSeconds);
// Update the flowing progress; call every loop iteration.
void updateProgressAnimation(Adafruit_ILI9341 &tft, uint16_t focusColor, uint16_t breakColor, int diameter = -1);
#endif
