#include "ui_design.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "ui/taskbar.h"
#include "ui/timer_display.h"
#include "ui/circle_progress.h" // retained for fallback
#include "ui/slot_progress.h"
#include "ui/theme_provider.h"
#include "features/buzzer.h"
#include "core/timer_controller.h"
#include "system/settings.h"
#include "system/statistics.h"

// Default beep configuration (can move to a config header later)
#define BUZZER_BEEP_MS 1000
#define BUZZER_DUTY_PERCENT 50

#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22
#define TFT_SCK  18
#define TFT_MISO 19
#define TFT_MOSI 23
#define EN_BUTTON 0 // GPIO0 for example, change if needed

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

Settings settings; // loads defaults then persistence
Statistics stats;  // persisted statistics
TimerController timer(25*60, 5*60); // will be replaced after settings load
Mode currentMode = FOCUS; // Mirror TimerController (will be removed when UI migrated)
bool lastButtonState = HIGH;
unsigned long lastButtonEventMs = 0; // for non-blocking debounce
const unsigned long BUTTON_DEBOUNCE_MS = 300;

void updateUI() {
  // Only refresh timer numbers
  int remaining = timer.getRemainingSeconds();
  int minutes = remaining / 60;
  int seconds = remaining % 60;
  drawTimerOptimized(tft, minutes, seconds);
}

#ifndef UNIT_TEST
void setup() {
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  tft.begin();
  tft.setRotation(1);
  pinMode(EN_BUTTON, INPUT_PULLUP);
  settings.load();
  stats.load();
  // Re-init timer with persisted settings
  timer = TimerController(settings.focusSeconds(), settings.breakSeconds());
  buzzerSetup();
  tft.fillScreen(COLOR_BG); // now black
  drawTaskbar(tft, currentMode);
  // Initialize new slot progress
  #if USE_SLOT_PROGRESS
    slotProgressReset();
    slotProgressStart(timer.getTotalSecondsForMode());
  #else
    float percent = 1.0f - ((float)timer.getRemainingSeconds() / timer.getTotalSecondsForMode());
    uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
    drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
    #if CONTINUOUS_PROGRESS
      startProgressCycle(timer.getTotalSecondsForMode());
    #endif
  #endif
  int minutes = timer.getRemainingSeconds() / 60;
  int seconds = timer.getRemainingSeconds() % 60;
  drawTimer(tft, minutes, seconds);
}

void loop() {
  // Button handling (toggle mode)
  bool buttonState = digitalRead(EN_BUTTON);
  if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long now = millis();
    if (now - lastButtonEventMs >= BUTTON_DEBOUNCE_MS) {
      // Button accepted (debounced)
      lastButtonEventMs = now;
      // Manual toggle
      timer.toggleMode();
      currentMode = timer.isFocus() ? FOCUS : BREAK;
      // Record completion stats when switching FROM focus
      if (!timer.isFocus()) {
        stats.recordFocusCompletion(settings.focusSeconds());
      } else {
        stats.recordBreakCompletion(settings.breakSeconds());
      }
  // Redraw base UI
  tft.fillScreen(COLOR_BG); // black on redraw
      drawTaskbar(tft, currentMode);
      resetTimerDisplayState();
      #if USE_SLOT_PROGRESS
        slotProgressReset();
        slotProgressStart(timer.getTotalSecondsForMode());
      #else
        resetCircleProgressState();
        float percent = 1.0f - ((float)timer.getRemainingSeconds() / timer.getTotalSecondsForMode());
        uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
        drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
        #if CONTINUOUS_PROGRESS
          startProgressCycle(timer.getTotalSecondsForMode());
        #endif
      #endif
      int minutes = timer.getRemainingSeconds() / 60;
      int seconds = timer.getRemainingSeconds() % 60;
      drawTimer(tft, minutes, seconds);
      // Audio cue: entering FOCUS = single short beep, entering BREAK = double beep
      if (currentMode == FOCUS) {
        buzzerBeep(150, BUZZER_DUTY_PERCENT);
      } else {
        buzzerDoubleBeep(120, 120, 120, BUZZER_DUTY_PERCENT);
      }
      // No blocking delay; buzzerService will run to stop beep exactly on time
    }
  }
  lastButtonState = buttonState;

  // Timer countdown
  if (timer.update()) {
    updateUI();
    // Auto mode switch handled inside timer; update currentMode mirror
    currentMode = timer.isFocus() ? FOCUS : BREAK;
    if (timer.getRemainingSeconds() == timer.getTotalSecondsForMode()) {
      // Just switched; restart progress trackers
      #if USE_SLOT_PROGRESS
        slotProgressReset();
        slotProgressStart(timer.getTotalSecondsForMode());
      #else
        resetCircleProgressState();
        #if CONTINUOUS_PROGRESS
          startProgressCycle(timer.getTotalSecondsForMode());
        #endif
      #endif
      // Stats: we switched FROM the other mode, record completion
      if (!timer.isFocus()) {
        stats.recordFocusCompletion(settings.focusSeconds());
      } else {
        stats.recordBreakCompletion(settings.breakSeconds());
      }
  // Redraw taskbar to reflect underline change
  drawTaskbar(tft, currentMode);
      // Beep on switch
  if (currentMode == FOCUS) buzzerBeep(150, BUZZER_DUTY_PERCENT); else buzzerDoubleBeep(120,120,120,BUZZER_DUTY_PERCENT);
    }
  }
  // Progress drawing
  #if USE_SLOT_PROGRESS
    slotProgressUpdate(tft, timer);
  #else
    #if CONTINUOUS_PROGRESS
      updateProgressAnimation(tft, COLOR_ACCENT_DUSTY_RED, COLOR_BREAK, PROGRESS_DIAMETER);
    #endif
  #endif
  // Non-blocking buzzer service
  buzzerService();
}
#endif // UNIT_TEST
