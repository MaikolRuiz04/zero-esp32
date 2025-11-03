#pragma once
#include <Adafruit_ILI9341.h>
#include "ui_design.h"
#include "core/timer_controller.h"

// Perimeter progress renderer for rounded slot.
// Call start when mode begins; update each frame.

void slotProgressReset();
void slotProgressStart(int totalSeconds);
void slotProgressUpdate(Adafruit_ILI9341 &tft, const TimerController &timer);
