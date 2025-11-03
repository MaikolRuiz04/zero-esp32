#ifndef PREFERENCES_MANAGER_H
#define PREFERENCES_MANAGER_H

#include <Preferences.h>

// Global preferences namespace
#define PREFS_NAMESPACE "pomodoro"

// Preferences keys prefixes for different parts of the application
#define PREF_CHECKBOX "cb."
#define PREF_STATISTICS "stats."
#define PREF_ANNIVERSARY "anniv."

extern Preferences preferences;

void initPreferences();

bool pref_getCheckbox(const char *key, bool defaultValue);
void pref_putCheckbox(const char *key, bool value);

unsigned int pref_getStatistic(const char *key, unsigned int defaultValue);
void pref_putStatistic(const char *key, unsigned int value);

unsigned long pref_getStatistic(const char *key, unsigned long defaultValue);
void pref_putStatistic(const char *key, unsigned long value);

#endif