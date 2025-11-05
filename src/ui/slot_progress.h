#pragma once
#include <TFT_eSPI.h>
#include "ui_design.h"
#include "core/timer_controller.h"

// Perimeter progress renderer for rounded slot.
// Call start when mode begins; update each frame.

void slotProgressReset();
void slotProgressStart(int totalSeconds);
void slotProgressUpdate(TFT_eSPI &tft, const TimerController &timer);
