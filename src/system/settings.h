#pragma once
#include <Arduino.h>

// Simple settings wrapper; future: persist via Preferences.
struct Settings {
    int focusMinutes = 25; // default
    int breakMinutes = 5;  // default

    int focusSeconds() const { return focusMinutes * 60; }
    int breakSeconds() const { return breakMinutes * 60; }

    void load(); // TODO: implement persistence
    void save(); // TODO: implement persistence
};
