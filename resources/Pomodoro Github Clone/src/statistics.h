#ifndef STATISTICS_H
#define STATISTICS_H

#include <Arduino.h>
#include <Preferences.h>

void incrementTotalCycles();
void incrementTotalTime(unsigned long ms);
void incrementTotalBreakTime(unsigned long ms);

void getStatistics(unsigned int *totalCycles, unsigned long *totalTime, unsigned long *totalBreakTime);
void resetStatistics();

#endif