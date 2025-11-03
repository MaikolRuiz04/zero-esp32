#include "statistics.h"
#include <Preferences.h>

static Preferences statsPrefs;

void Statistics::load() {
    if (!statsPrefs.begin("zero_stats", true)) return;
    focusMinutesTotal = statsPrefs.getULong("focusMinTot", focusMinutesTotal);
    breakMinutesTotal = statsPrefs.getULong("breakMinTot", breakMinutesTotal);
    focusCycles = statsPrefs.getUInt("focusCycles", focusCycles);
    statsPrefs.end();
}

void Statistics::save() const {
    if (!statsPrefs.begin("zero_stats", false)) return;
    statsPrefs.putULong("focusMinTot", focusMinutesTotal);
    statsPrefs.putULong("breakMinTot", breakMinutesTotal);
    statsPrefs.putUInt("focusCycles", focusCycles);
    statsPrefs.end();
}
