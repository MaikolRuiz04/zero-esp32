#pragma once
#include <Adafruit_ILI9341.h>
enum Mode { FOCUS, BREAK };
void drawTaskbar(Adafruit_ILI9341 &tft, Mode mode);
