#include "statistics.h"
#include <Preferences.h>

static Preferences statsPrefs;

void Statistics::load() {
    bool ok = statsPrefs.begin("zero_stats", true);
    Serial.print("Stats NVS begin (read): ");
    Serial.println(ok ? "success" : "fail");
    if (!ok) {
        Serial.println("Stats NVS missing, saving defaults to create namespace...");
        save();
        return;
    }
    focusMinutesTotal = statsPrefs.getULong("focusMinTot", focusMinutesTotal);
    breakMinutesTotal = statsPrefs.getULong("breakMinTot", breakMinutesTotal);
    focusCycles = statsPrefs.getUInt("focusCycles", focusCycles);
    statsPrefs.end();
}

void Statistics::save() const {
    bool ok = statsPrefs.begin("zero_stats", false);
    Serial.print("Stats NVS begin (write): ");
    Serial.println(ok ? "success" : "fail");
    if (!ok) return;
    statsPrefs.putULong("focusMinTot", focusMinutesTotal);
    statsPrefs.putULong("breakMinTot", breakMinutesTotal);
    statsPrefs.putUInt("focusCycles", focusCycles);
    statsPrefs.end();
}
