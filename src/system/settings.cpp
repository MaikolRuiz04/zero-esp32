#include "settings.h"
#include <Preferences.h>

static Preferences prefs;

void Settings::load() {
    if (!prefs.begin("zero", true)) return; // read-only
    focusMinutes = prefs.getUInt("focusMin", focusMinutes);
    breakMinutes = prefs.getUInt("breakMin", breakMinutes);
    prefs.end();
}

void Settings::save() {
    if (!prefs.begin("zero", false)) return; // write
    prefs.putUInt("focusMin", focusMinutes);
    prefs.putUInt("breakMin", breakMinutes);
    prefs.end();
}
