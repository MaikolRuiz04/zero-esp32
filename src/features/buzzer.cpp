#include "features/buzzer.h"

static bool s_active = false;
static unsigned long s_endMs = 0;
static bool s_twoTone = false;
static unsigned long s_secondStartDelayMs = 0;
static uint32_t s_freqB = 0; static uint8_t s_dutyB = 0; static uint32_t s_durB = 0;
// Double beep state
static bool s_doubleBeep = false;
static uint8_t s_doubleStage = 0; // 0 idle,1 first on,2 gap,3 second on
static uint16_t s_on2Ms = 0; static uint16_t s_gapMs = 0; static uint8_t s_doubleDuty = 0;

void buzzerSetup() {
  ledcSetup(BUZZER_CHANNEL, BUZZER_GATE_FREQ, BUZZER_PWM_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);
  s_active = false;
}

void buzzerBeep(uint32_t durationMs, uint8_t dutyPercent) {
  if (dutyPercent > 100) dutyPercent = 100;
  uint32_t duty = ((1UL << BUZZER_PWM_RES) - 1) * dutyPercent / 100;
  ledcWrite(BUZZER_CHANNEL, duty);
  s_active = true;
  s_endMs = millis() + durationMs;
  s_twoTone = false;
}

void buzzerService() {
  unsigned long now = millis();
  if (s_active && now >= s_endMs) {
    if (s_twoTone && s_secondStartDelayMs == 0) {
      // Start second tone immediately
      s_secondStartDelayMs = 1; // marker to start next loop
    } else if (s_twoTone && s_secondStartDelayMs == 1) {
      // Launch second tone
      ledcSetup(BUZZER_CHANNEL, s_freqB, BUZZER_PWM_RES);
      uint32_t duty = ((1UL << BUZZER_PWM_RES) - 1) * s_dutyB / 100;
      ledcWrite(BUZZER_CHANNEL, duty);
      s_endMs = now + s_durB;
      s_secondStartDelayMs = 2; // running second
    } else {
      ledcWrite(BUZZER_CHANNEL, 0);
      s_active = false;
      s_twoTone = false;
      if (s_doubleBeep) {
        // Progress double beep stages
        if (s_doubleStage == 1) { // finished first on -> start gap
          s_doubleStage = 2;
          s_endMs = now + s_gapMs;
          s_active = true; // using s_active to time gap
        } else if (s_doubleStage == 2) { // gap finished -> second on
          s_doubleStage = 3;
          uint32_t duty = ((1UL << BUZZER_PWM_RES) - 1) * s_doubleDuty / 100;
          ledcWrite(BUZZER_CHANNEL, duty);
          s_endMs = now + s_on2Ms;
          s_active = true;
        } else if (s_doubleStage == 3) { // second on finished
          s_doubleBeep = false;
          s_doubleStage = 0;
        }
      }
    }
  }
}

void buzzerStop() {
  ledcWrite(BUZZER_CHANNEL, 0);
  s_active = false;
}

bool buzzerIsActive() { return s_active; }

void buzzerBeepFreq(uint32_t durationMs, uint8_t dutyPercent, uint32_t gateFreq) {
  if (dutyPercent > 100) dutyPercent = 100;
  ledcSetup(BUZZER_CHANNEL, gateFreq, BUZZER_PWM_RES);
  uint32_t duty = ((1UL << BUZZER_PWM_RES) - 1) * dutyPercent / 100;
  ledcWrite(BUZZER_CHANNEL, duty);
  s_active = true;
  s_twoTone = false;
  s_endMs = millis() + durationMs;
}

void buzzerTwoTone(uint32_t freqA, uint8_t dutyA, uint32_t durAms,
                   uint32_t freqB, uint8_t dutyB, uint32_t durBms) {
  if (dutyA > 100) dutyA = 100; if (dutyB > 100) dutyB = 100;
  if (durAms == 0) return;
  ledcSetup(BUZZER_CHANNEL, freqA, BUZZER_PWM_RES);
  uint32_t duty = ((1UL << BUZZER_PWM_RES) - 1) * dutyA / 100;
  ledcWrite(BUZZER_CHANNEL, duty);
  s_active = true;
  s_endMs = millis() + durAms;
  if (durBms > 0) {
    s_twoTone = true;
    s_secondStartDelayMs = 0; // will transition when first completes
    s_freqB = freqB; s_dutyB = dutyB; s_durB = durBms;
  } else {
    s_twoTone = false;
  }
}

void buzzerDoubleBeep(uint16_t on1Ms, uint16_t gapMs, uint16_t on2Ms, uint8_t dutyPercent) {
  if (dutyPercent > 100) dutyPercent = 100;
  s_doubleBeep = true;
  s_doubleStage = 1;
  s_gapMs = gapMs;
  s_on2Ms = on2Ms;
  s_doubleDuty = dutyPercent;
  s_twoTone = false; // cancel any two-tone in progress
  // Start first beep using existing channel config
  uint32_t duty = ((1UL << BUZZER_PWM_RES) - 1) * dutyPercent / 100;
  ledcWrite(BUZZER_CHANNEL, duty);
  s_active = true;
  s_endMs = millis() + on1Ms;
}
