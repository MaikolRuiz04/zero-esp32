#pragma once
#include <TFT_eSPI.h>
#include "core/timer_controller.h"
#include "ui_design.h"

// Centralized color decisions for modes / states.
struct ThemeProvider {
    // Slot fill: use different palette slot color for each mode
    static uint16_t slotFillColor(const TimerController &t) {
        return t.getMode() == TimerMode::Focus ? COLOR_SLOT_FOCUS : COLOR_SLOT_BREAK;
    }
    // Progress bar: always anti-flash white
    static uint16_t progressColor(const TimerController &) {
        return COLOR_TIMER_TEXT;
    }
    // Timer text: anti-flash white
    static uint16_t textColorTimer(const TimerController &) { return COLOR_TIMER_TEXT; }
    // Underline: always anti-flash white
    static uint16_t underlineColor(const TimerController &) {
        return COLOR_TIMER_TEXT;
    }
};
