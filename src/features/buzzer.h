// Buzzer control module for active buzzer (TMB12A05 via NPN transistor)
// Provides simple non-blocking 1-shot beeps with pseudo volume (PWM gating)
#pragma once
#include <Arduino.h>

// Default configuration (can be overridden before including this header if needed)
#ifndef BUZZER_PIN
#define BUZZER_PIN 25
#endif

#ifndef BUZZER_CHANNEL
#define BUZZER_CHANNEL 0
#endif

#ifndef BUZZER_GATE_FREQ
#define BUZZER_GATE_FREQ 1200 // Hz PWM gate frequency (active buzzer self-oscillates internally)
#endif

#ifndef BUZZER_PWM_RES
#define BUZZER_PWM_RES 8 // bits
#endif


// Initialize LEDC channel and attach pin (call once in setup)
void buzzerSetup();

// Start a non-blocking beep (duration in ms, dutyPercent 0-100). Overwrites any current beep.
void buzzerBeep(uint32_t durationMs, uint8_t dutyPercent);

// Call frequently in loop() to stop beeps when time elapses.
void buzzerService();

// Force stop (immediate off)
void buzzerStop();

// Returns true if a beep is currently active.
bool buzzerIsActive();

// Beep with a specific gate frequency (reconfigures LEDC on the fly)
void buzzerBeepFreq(uint32_t durationMs, uint8_t dutyPercent, uint32_t gateFreq);

// Simple two-tone sequence (tone A then tone B). Non-blocking; second starts automatically.
// freq/duty/duration for tone A then B. If second duration is 0, only first plays.
void buzzerTwoTone(uint32_t freqA, uint8_t dutyA, uint32_t durAms,
				   uint32_t freqB, uint8_t dutyB, uint32_t durBms);

// Simple double-beep (on-gap-on). All durations in ms.
// Example: buzzerDoubleBeep(120,120,120,50) -> beep 120ms, pause 120ms, beep 120ms
void buzzerDoubleBeep(uint16_t on1Ms, uint16_t gapMs, uint16_t on2Ms, uint8_t dutyPercent);

// (C linkage wrappers removed; plain C++ functions)
