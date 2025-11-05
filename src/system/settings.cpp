#include "settings.h"
#include <Preferences.h>

static Preferences prefs;

void Settings::load() {
    bool ok = prefs.begin("zero", true);
    Serial.print("Settings NVS begin (read): ");
    Serial.println(ok ? "success" : "fail");
    if (!ok) {
        Serial.println("Settings NVS missing, saving defaults to create namespace...");
        save();
        return;
    }
    focusMinutes = prefs.getUInt("focusMin", focusMinutes);
    breakMinutes = prefs.getUInt("breakMin", breakMinutes);
    prefs.end();
}

void Settings::save() {
    bool ok = prefs.begin("zero", false);
    Serial.print("Settings NVS begin (write): ");
    Serial.println(ok ? "success" : "fail");
    if (!ok) return; // write
    prefs.putUInt("focusMin", focusMinutes);
    prefs.putUInt("breakMin", breakMinutes);
    prefs.end();
}
