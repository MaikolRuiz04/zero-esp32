#pragma once
#include <TFT_eSPI.h>
void drawTimer(TFT_eSPI &tft, int minutes, int seconds);
// Use only drawTimer for all timer updates.
void resetTimerDisplayState();
