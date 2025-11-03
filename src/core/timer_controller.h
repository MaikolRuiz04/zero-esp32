#pragma once
#include <Arduino.h>

// High-level timer controller replacing scattered globals.
// Tracks focus/break durations, mode transitions, and countdown.

enum class TimerMode { Focus, Break };

class TimerController {
private:
    TimerMode mode;
    int focusSeconds;
    int breakSeconds;
    int remainingSeconds;
    unsigned long lastTickMs = 0; // for 1Hz decrement
public:
    TimerController(int focusSec, int breakSec)
        : mode(TimerMode::Focus), focusSeconds(focusSec), breakSeconds(breakSec), remainingSeconds(focusSec) {}

    void reset(TimerMode m) {
        mode = m;
        remainingSeconds = (mode == TimerMode::Focus ? focusSeconds : breakSeconds);
    }

    void toggleMode() {
        if (mode == TimerMode::Focus) reset(TimerMode::Break); else reset(TimerMode::Focus);
    }

    bool isFocus() const { return mode == TimerMode::Focus; }
    TimerMode getMode() const { return mode; }
    int getRemainingSeconds() const { return remainingSeconds; }
    int getTotalSecondsForMode() const { return (mode == TimerMode::Focus ? focusSeconds : breakSeconds); }

    // returns true if a second decrement occurred
    bool update() {
        unsigned long now = millis();
        if (now - lastTickMs >= 1000) {
            lastTickMs = now;
            if (remainingSeconds > 0) {
                --remainingSeconds;
                return true;
            } else {
                // Auto-switch mode
                toggleMode();
                return true; // force UI refresh
            }
        }
        return false;
    }
};
