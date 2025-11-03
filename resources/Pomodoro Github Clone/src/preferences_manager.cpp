#include "preferences_manager.h"

Preferences preferences;

void initPreferences()
{
    // preferences.begin(PREFS_NAMESPACE, false);
}

bool pref_getCheckbox(const char *key, bool defaultValue)
{
    preferences.begin(PREFS_NAMESPACE, true);
    char prefKey[32];
    snprintf(prefKey, sizeof(prefKey), "%s%s", PREF_CHECKBOX, key);

    auto v = preferences.getBool(prefKey, defaultValue);

    preferences.end();

    return v;
}

void pref_putCheckbox(const char *key, bool value)
{
    preferences.begin(PREFS_NAMESPACE, false);
    char prefKey[32];
    snprintf(prefKey, sizeof(prefKey), "%s%s", PREF_CHECKBOX, key);
    preferences.putBool(prefKey, value);
    preferences.end();
}

unsigned int pref_getStatistic(const char *key, unsigned int defaultValue)
{
    preferences.begin(PREFS_NAMESPACE, true);
    char prefKey[32];
    snprintf(prefKey, sizeof(prefKey), "%s%s", PREF_STATISTICS, key);

    auto v = preferences.getUInt(prefKey, defaultValue);

    preferences.end();
    return v;
}

void pref_putStatistic(const char *key, unsigned int value)
{
    preferences.begin(PREFS_NAMESPACE, false);
    char prefKey[32];
    snprintf(prefKey, sizeof(prefKey), "%s%s", PREF_STATISTICS, key);
    preferences.putUInt(prefKey, value);
    preferences.end();
}

unsigned long pref_getStatistic(const char *key, unsigned long defaultValue)
{
    preferences.begin(PREFS_NAMESPACE, true);
    char prefKey[32];
    snprintf(prefKey, sizeof(prefKey), "%s%s", PREF_STATISTICS, key);

    auto v = preferences.getULong(prefKey, defaultValue);

    preferences.end();
    return v;
}

void pref_putStatistic(const char *key, unsigned long value)
{
    preferences.begin(PREFS_NAMESPACE, false);
    char prefKey[32];
    snprintf(prefKey, sizeof(prefKey), "%s%s", PREF_STATISTICS, key);
    preferences.putULong(prefKey, value);
    preferences.end();
}