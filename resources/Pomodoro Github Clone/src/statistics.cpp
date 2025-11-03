#include "statistics.h"
#include "preferences_manager.h"

extern Preferences preferences;

void incrementTotalCycles()
{
    Serial.printf("Statistics: incrementTotalCycles (+1)\n");
    unsigned int totalCycles = pref_getStatistic("tcs", (unsigned int)0);
    totalCycles++;
    pref_putStatistic("tcs", totalCycles);
}

void incrementTotalTime(unsigned long ms)
{
    Serial.printf("Statistics: incrementTotalTime (%lu [+%lu])\n", ms, ms / 1000 / 60);
    unsigned long totalTime = pref_getStatistic("tt", (unsigned long)0);
    totalTime += ms / 1000 / 60;
    pref_putStatistic("tt", totalTime);
}

void incrementTotalBreakTime(unsigned long ms)
{
    Serial.printf("Statistics: incrementTotalBreakTime (%lu [+%lu])\n", ms, ms / 1000 / 60);
    unsigned long totalBreakTime = pref_getStatistic("tbt", (unsigned long)0);
    totalBreakTime += ms / 1000 / 60;
    pref_putStatistic("tbt", totalBreakTime);
}

void getStatistics(unsigned int *totalCycles, unsigned long *totalTime, unsigned long *totalBreakTime)
{
    *totalCycles = pref_getStatistic("tcs", (unsigned int)0);
    *totalTime = pref_getStatistic("tt", (unsigned long)0);
    *totalBreakTime = pref_getStatistic("tbt", (unsigned long)0);

    Serial.printf("Statistics: getStatistics: totalCycles=%d, totalTime=%lu, totalBreakTime=%lu\n", *totalCycles, *totalTime, *totalBreakTime);
}

void resetStatistics()
{
    Serial.println("Statistics: resetStatistics");
    pref_putStatistic("tcs", (unsigned int)0);
    pref_putStatistic("tt", (unsigned long)0);
    pref_putStatistic("tbt", (unsigned long)0);
}