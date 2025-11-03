#pragma once
#include <Arduino.h>

// Tracks cumulative focus/break minutes (persisted later).
struct Statistics {
    unsigned long focusMinutesTotal = 0;
    unsigned long breakMinutesTotal = 0;
    unsigned int focusCycles = 0;

    void load();
    void save() const;

    void recordFocusCompletion(int seconds) {
        focusMinutesTotal += seconds / 60; // whole minutes
        ++focusCycles;
        save();
    }

    void recordBreakCompletion(int seconds) {
        breakMinutesTotal += seconds / 60;
        save();
    }

    void reset() {
        focusMinutesTotal = 0;
        breakMinutesTotal = 0;
        focusCycles = 0;
        save();
    }
};
