#pragma once
#include <TFT_eSPI.h>
enum Mode { FOCUS, BREAK };
// Overload for backward compatibility
void drawTaskbar(TFT_eSPI &tft, Mode mode);
// New version with verticalPad
void drawTaskbar(TFT_eSPI &tft, Mode mode, int verticalPad);
