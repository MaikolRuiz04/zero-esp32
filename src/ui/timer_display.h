#pragma once
#include <Adafruit_ILI9341.h>
void drawTimer(Adafruit_ILI9341 &tft, int minutes, int seconds);
// Optimized: only redraw digits that changed since last call
void drawTimerOptimized(Adafruit_ILI9341 &tft, int minutes, int seconds);
// Reset internal state so next optimized draw repaints all digits
void resetTimerDisplayState();
