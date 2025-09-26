#include "ui_design.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "ui/taskbar.h"
#include "ui/timer_display.h"
#include "ui/circle_progress.h"
#include "features/buzzer.h"
#include "features/rotary_encoder.h"
#include <esp_system.h>

// Default beep configuration (can move to a config header later)
#define BUZZER_BEEP_MS 1000
#define BUZZER_DUTY_PERCENT 50

#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22
#define TFT_SCK  18
#define TFT_MISO 19
#define TFT_MOSI 23

// Rotary Encoder (KY-040) pin mapping
#define ENC_CLK 32
#define ENC_DT  33
#define ENC_SW  27   // Push switch (active LOW)

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// Running mode (the one whose timer is actually counting down)
Mode currentMode = FOCUS;
// UI selection (what the rotary encoder underline is on). Only applied when encoder button is pressed.
Mode selectedMode = FOCUS;
int focusSeconds = 25 * 60;
int breakSeconds = 5 * 60;
int timerSeconds = focusSeconds;

// Encoder state handled by rotary_encoder module

// Enable verbose debug (set to 1 for serial diagnostics of encoder + resets)
#ifndef APP_DEBUG
#define APP_DEBUG 1
#endif

// Enable periodic raw switch state logging for diagnostics (0 = off, 1 = on)
#ifndef APP_SW_DEBUG
#define APP_SW_DEBUG 1
#endif

static void commitModeSwitch() {
  // Redraw static UI for new currentMode (skip heavy work if in minimal diagnostic mode)
#if MINIMAL_DIAG
  tft.fillScreen(ILI9341_BLACK);
#else
  tft.fillScreen(ILI9341_BLACK);
  drawTaskbar(tft, selectedMode);
  resetCircleProgressState();
  resetTimerDisplayState();
  float percent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
  uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
  drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
  int minutes = timerSeconds / 60;
  int seconds = timerSeconds % 60;
  drawTimer(tft, minutes, seconds);
#endif
  if (currentMode == FOCUS) buzzerBeep(150, BUZZER_DUTY_PERCENT); else buzzerDoubleBeep(120, 120, 120, BUZZER_DUTY_PERCENT);
}

static void fullUIDraw() {
  tft.fillScreen(ILI9341_BLACK);
#if !MINIMAL_DIAG
  drawTaskbar(tft, selectedMode);
  resetCircleProgressState();
  resetTimerDisplayState();
  float percent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
  uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
  drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
  int minutes = timerSeconds / 60;
  int seconds = timerSeconds % 60;
  drawTimer(tft, minutes, seconds);
#endif
}

static const char* resetReasonStr(esp_reset_reason_t r){
  switch(r){
    case ESP_RST_POWERON: return "POWERON";
    case ESP_RST_EXT: return "EXT";
    case ESP_RST_SW: return "SW";
    case ESP_RST_PANIC: return "PANIC";
    case ESP_RST_INT_WDT: return "INT_WDT";
    case ESP_RST_TASK_WDT: return "TASK_WDT";
    case ESP_RST_WDT: return "WDT";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
    case ESP_RST_BROWNOUT: return "BROWNOUT";
    case ESP_RST_SDIO: return "SDIO";
    default: return "UNKNOWN";
  }
}

void updateUI() {
  // Only refresh timer numbers / or minimal heartbeat
#if MINIMAL_DIAG
  static int hb = 0; hb = (hb + 1) & 0x1F;
  tft.drawPixel(2 + hb, 2, ILI9341_WHITE);
#else
  int minutes = timerSeconds / 60;
  int seconds = timerSeconds % 60;
  drawTimerOptimized(tft, minutes, seconds);
#endif
}

#ifndef UNIT_TEST
void setup() {
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  tft.begin(TFT_SPI_HZ);
  tft.setRotation(1);
#if APP_DEBUG
  Serial.begin(115200);
  delay(50);
  Serial.println("\n[BOOT] Application start");
  auto rr = esp_reset_reason();
  Serial.printf("Reset reason: %d (%s)\n", (int)rr, resetReasonStr(rr));
  if (rr == ESP_RST_BROWNOUT) {
    Serial.println("[DIAG] Brownout detected: supply dipped below threshold. Check power, wiring, or noise from encoder.");
  }
#endif
  // Initialize rotary encoder (step debounce, press debounce, press stable window)
  encoderInit(ENC_CLK, ENC_DT, ENC_SW,
              ENCODER_CFG_STEP_DEBOUNCE_MS,
              ENCODER_CFG_PRESS_DEBOUNCE_MS,
              ENCODER_CFG_PRESS_STABLE_MS);
  buzzerSetup();
  // Draw static UI elements once
  tft.fillScreen(ILI9341_BLACK);
#if !MINIMAL_DIAG
  drawTaskbar(tft, selectedMode);
  float percent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
  uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
  drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
  // Draw timer
  int minutes = timerSeconds / 60;
  int seconds = timerSeconds % 60;
  drawTimer(tft, minutes, seconds);
#endif
#if APP_DEBUG
  Serial.println("[BOOT] UI initialised");
#endif
}

void loop() {
  // Detect brownout after boot and attempt single re-init (in case bus got unstable)
  static bool attemptedRecovery = false;
  if (!attemptedRecovery) {
    // If last reset was brownout, we already logged it in setup; no action needed now.
    attemptedRecovery = true; // future: could trigger lighter init
  }

  // Track UI activity for watchdog
  static unsigned long lastUIDrawMs = 0;
  // We count a 'draw' as any taskbar redraw or timer/progress update
  // (We update this in places where drawing occurs)
  // --- Rotary Encoder Poll ---
  EncoderEvent ev = encoderPoll();
  if (ev.type == ENCODER_ROTATE) {
    int dir = ev.direction;
    if (ENCODER_CFG_INVERT_DIRECTION) dir = -dir;
    // With only two items, any rotation toggles; later add index logic if >2
    if (dir != 0) selectedMode = (selectedMode == FOCUS ? BREAK : FOCUS);
    drawTaskbar(tft, selectedMode);
#if APP_DEBUG
  lastUIDrawMs = millis();
#endif
#if APP_DEBUG
    Serial.printf("[ENC] ROT dir=%d selected=%d current=%d\n", ev.direction, (int)selectedMode, (int)currentMode);
#endif
  } else if (ev.type == ENCODER_PRESS) {
    if (selectedMode != currentMode) {
      currentMode = selectedMode;
      timerSeconds = (currentMode == FOCUS ? focusSeconds : breakSeconds);
#if APP_DEBUG
      Serial.printf("[ENC] PRESS commit -> mode=%d\n", (int)currentMode);
#endif
      commitModeSwitch();
  lastUIDrawMs = millis();
    } else {
#if APP_DEBUG
      Serial.println("[ENC] PRESS ignored (same mode)");
#endif
    }
  }

  // Raw switch state transition logging (helps if PRESS events never fire)
  static int lastRawSw = -1;
  int rawSw = encoderSwitchStable() ? 1 : 0; // stable filtered state
  if (lastRawSw != -1 && rawSw != lastRawSw) {
#if APP_DEBUG
    Serial.printf("[ENC] SW raw %s\n", rawSw?"DOWN":"UP");
#endif
  }
  lastRawSw = rawSw;

  // Raw encoder line sampling for diagnostics
  static unsigned long lastPinSample = 0;
  if (millis() - lastPinSample > 250) {
    lastPinSample = millis();
    int clkLevel = digitalRead(ENC_CLK);
    int dtLevel  = digitalRead(ENC_DT);
    int swLevel  = digitalRead(ENC_SW);
#if APP_DEBUG
    Serial.printf("[PIN] CLK=%d DT=%d SW=%d\n", clkLevel, dtLevel, swLevel);
#endif
  }

#if APP_SW_DEBUG
  // Periodic raw SW sample (helps if no edge captured yet)
  static unsigned long lastDbg = 0; if (millis() - lastDbg > 500) {
    Serial.printf("[DBG] SW(stable)=%d\n", rawSw);
    lastDbg = millis();
  }
#endif

  // Timer countdown
  static unsigned long lastTick = 0;
  static float lastProgressPercent = -1.0f; // for smooth delta drawing
  if (millis() - lastTick >= 1000) {
    if (timerSeconds > 0) {
      timerSeconds--;
      updateUI();
  lastUIDrawMs = millis();
#if SMOOTH_PROGRESS
      // Draw incremental arc every second (smooth mode)
      int totalForMode = (currentMode == FOCUS ? focusSeconds : breakSeconds);
      float percent = 1.0f - ((float)timerSeconds / totalForMode);
      uint16_t arcColor = (currentMode == FOCUS) ? COLOR_ACCENT_DUSTY_RED : COLOR_BREAK;
      if (!MINIMAL_DIAG) {
        if (lastProgressPercent < 0) {
          drawCircleProgress(tft, percent, arcColor, PROGRESS_DIAMETER);
        } else {
          drawCircleProgressDelta(tft, lastProgressPercent, percent, arcColor, PROGRESS_DIAMETER);
        }
        lastUIDrawMs = millis();
      }
      lastProgressPercent = percent;
#endif
    } else {
      // Switch mode automatically when timer runs out
      if (currentMode == FOCUS) { currentMode = BREAK; timerSeconds = breakSeconds; }
      else { currentMode = FOCUS; timerSeconds = focusSeconds; }
      // Keep selection synced with auto cycle so underline follows running mode
      selectedMode = currentMode;
      // Redraw static UI elements on mode switch
  commitModeSwitch(); // will skip heavy draws if MINIMAL_DIAG
      lastProgressPercent = 1.0f - ((float)timerSeconds / (currentMode == FOCUS ? focusSeconds : breakSeconds));
  lastUIDrawMs = millis();
#if APP_DEBUG
      Serial.printf("[AUTO] Mode cycled -> %d\n", (int)currentMode);
#endif
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

  // Display watchdog: attempt recovery if no UI draw for threshold
  static bool inRecovery = false;
  unsigned long nowMs = millis();
  if (!inRecovery && lastUIDrawMs != 0 && (nowMs - lastUIDrawMs) > DISPLAY_WATCHDOG_MS) {
#if APP_DEBUG
    Serial.println("[WDOG] Display inactivity detected, reinitializing TFT");
#endif
    inRecovery = true;
    tft.begin(TFT_SPI_HZ);
    tft.setRotation(1);
    fullUIDraw();
    lastUIDrawMs = millis();
    inRecovery = false;
  }
}
#endif // UNIT_TEST
