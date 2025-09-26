#include "ui_design.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "ui/taskbar.h"
#include "ui/timer_display.h"
#include "ui/circle_progress.h"
#include "features/buzzer.h"

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

Mode currentMode = FOCUS;
int focusSeconds = 25 * 60;
int breakSeconds = 5 * 60;
int timerSeconds = focusSeconds;
bool lastButtonState = HIGH;
unsigned long lastButtonEventMs = 0; // for non-blocking debounce
const unsigned long BUTTON_DEBOUNCE_MS = 300;

void updateUI() {
  // Only refresh timer numbers
  int minutes = timerSeconds / 60;
  int seconds = timerSeconds % 60;
  drawTimerOptimized(tft, minutes, seconds);
}

void setup() {
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  tft.begin();
  tft.setRotation(1);
  pinMode(EN_BUTTON, INPUT_PULLUP);
  buzzerSetup();
  // Draw static UI elements once
  tft.fillScreen(ILI9341_BLACK);
  drawTaskbar(tft, currentMode);
  float percent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
  uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
  drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
  // Draw timer
  int minutes = timerSeconds / 60;
  int seconds = timerSeconds % 60;
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
      if (currentMode == FOCUS) {
        currentMode = BREAK;
        timerSeconds = breakSeconds;
      } else {
        currentMode = FOCUS;
        timerSeconds = focusSeconds;
      }
      // Redraw static UI elements on mode switch
      tft.fillScreen(ILI9341_BLACK);
      drawTaskbar(tft, currentMode);
      resetCircleProgressState();
      resetTimerDisplayState();
      float percent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
      uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
      drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
      // Draw timer
      int minutes = timerSeconds / 60;
      int seconds = timerSeconds % 60;
      drawTimer(tft, minutes, seconds);
      // Audio cue: entering FOCUS = single short beep, entering BREAK = double beep
      if (currentMode == FOCUS) {
        buzzerBeep(150, BUZZER_DUTY_PERCENT); // single concise confirmation
      } else {
        buzzerDoubleBeep(120, 120, 120, BUZZER_DUTY_PERCENT); // break indicator
      }
      // No blocking delay; buzzerService will run to stop beep exactly on time
    }
  }
  lastButtonState = buttonState;

  // Timer countdown
  static unsigned long lastTick = 0;
  static float lastProgressPercent = -1.0f; // for smooth delta drawing
  if (millis() - lastTick >= 1000) {
    if (timerSeconds > 0) {
      timerSeconds--;
      updateUI();
#if SMOOTH_PROGRESS
      // Draw incremental arc every second (smooth mode)
      int totalForMode = (currentMode == FOCUS ? focusSeconds : breakSeconds);
      float percent = 1.0f - ((float)timerSeconds / totalForMode);
      uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
      if (lastProgressPercent < 0) {
        drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
      } else {
        drawCircleProgressDelta(tft, lastProgressPercent, percent, arcColor, PROGRESS_DIAMETER);
      }
      lastProgressPercent = percent;
#endif
    } else {
      // Switch mode automatically when timer runs out
      if (currentMode == FOCUS) {
        currentMode = BREAK;
        timerSeconds = breakSeconds;
      } else {
        currentMode = FOCUS;
        timerSeconds = focusSeconds;
      }
      // Redraw static UI elements on mode switch
      tft.fillScreen(ILI9341_BLACK);
      drawTaskbar(tft, currentMode);
      resetCircleProgressState();
  resetTimerDisplayState();
    float percent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
    uint16_t arcColor2 = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
    drawCircleProgress(tft, percent, arcColor2, PROGRESS_DIAMETER);
    lastProgressPercent = percent;
      if (currentMode == FOCUS) {
        buzzerBeep(150, BUZZER_DUTY_PERCENT);
      } else {
        buzzerDoubleBeep(120, 120, 120, BUZZER_DUTY_PERCENT);
      }
      int minutes = timerSeconds / 60;
      int seconds = timerSeconds % 60;
      drawTimer(tft, minutes, seconds);
    }
    lastTick = millis();
  }
#if !SMOOTH_PROGRESS
  // Segmented update mode
  int totalForMode = (currentMode == FOCUS ? focusSeconds : breakSeconds);
  int segmentLengthSec = totalForMode / PROGRESS_SEGMENTS;
  if (segmentLengthSec < 1) segmentLengthSec = 1;
  static int lastSegmentIndex = -1;
  int elapsed = totalForMode - timerSeconds;
  int currentSegmentIndex = elapsed / segmentLengthSec;
  if (currentSegmentIndex != lastSegmentIndex) {
    float percent = 1.0f - ((float)timerSeconds / totalForMode);
    uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
    drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
    lastSegmentIndex = currentSegmentIndex;
  }
#endif
  // Non-blocking buzzer service
  buzzerService();
}
