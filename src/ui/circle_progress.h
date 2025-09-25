#pragma once
#include <Adafruit_ILI9341.h>
void drawCircleProgress(Adafruit_ILI9341 &tft, float percent, uint16_t color, int diameter = -1);
