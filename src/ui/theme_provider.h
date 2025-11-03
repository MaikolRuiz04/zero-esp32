#pragma once
#include <Adafruit_ILI9341.h>
#include "core/timer_controller.h"
#include "ui_design.h"

// Centralized color decisions for modes / states.
struct ThemeProvider {
    static uint16_t slotFillColor(const TimerController &t) {
        return t.getMode() == TimerMode::Focus ? COLOR_ACCENT_OCHRE : COLOR_SLOT;
    }
    static uint16_t progressColor(const TimerController &t) {
        return t.getMode() == TimerMode::Focus ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
    }
    static uint16_t textColorTimer(const TimerController &) { return 0xFFFF; }
    static uint16_t underlineColor(const TimerController &t) {
        return t.getMode() == TimerMode::Focus ? COLOR_UNDERLINE_FOCUS : COLOR_UNDERLINE_BREAK;
    }
};
