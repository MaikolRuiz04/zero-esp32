#include "ui_design.h"
#include <Arduino.h>
#include <SPI.h>
// TFT_eSPI migration
#include <TFT_eSPI.h>
#include "ui/taskbar.h"
#include "ui/timer_display.h"
#include "ui/circle_progress.h" // retained for fallback
#include "ui/slot_progress.h"
#include "ui/theme_provider.h"
#include "features/buzzer.h"
#include "core/timer_controller.h"
#include "system/settings.h"
#include <ESP32Encoder.h>
#include <esp_sleep.h>

// Default beep configuration (can move to a config header later)
#define BUZZER_BEEP_MS 1000
#define BUZZER_DUTY_PERCENT 50

#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22
#define TFT_SCK  18
#define TFT_MISO 19
#define TFT_MOSI 23

#define TFT_BACKLIGHT 12 // GPIO12 for backlight control

// Rotary Encoder Pins
#define ROTARY_CLK 27  // GPIO27 (A, interrupt)
#define ROTARY_DT  26  // GPIO26 (B)
#define ROTARY_SW  32  // GPIO32 (button)

TFT_eSPI tft = TFT_eSPI();

Settings settings; // loads defaults then persistence
TimerController timer(25*60, 5*60); // will be replaced after settings load
Mode currentMode = FOCUS; // Mirror TimerController (will be removed when UI migrated)
bool lastButtonState = HIGH;
unsigned long lastButtonEventMs = 0; // for non-blocking debounce
const unsigned long BUTTON_DEBOUNCE_MS = 300;



volatile bool rotaryButtonPressed = false;
volatile unsigned long lastRotaryButtonMs = 0;
const unsigned long ROTARY_BUTTON_DEBOUNCE_MS = 300;

// Button press/release tracking
bool rotaryButtonDown = false;
unsigned long rotaryButtonDownMs = 0;
const unsigned long ROTARY_BUTTON_LONGPRESS_MS = 5000;

volatile int debouncedCount = 0;
volatile int lastCount = 0;
volatile unsigned long lastEncoderUpdate = 0;
const unsigned long ENCODER_DEBOUNCE_MS = 10;

ESP32Encoder encoder;

void IRAM_ATTR handleRotaryButton() {
  unsigned long now = millis();
  if (now - lastRotaryButtonMs > ROTARY_BUTTON_DEBOUNCE_MS) {
    // On falling edge (button down)
    rotaryButtonDown = true;
    rotaryButtonDownMs = now;
    lastRotaryButtonMs = now;
  }
}

void IRAM_ATTR checkEncoderPosition() {
  unsigned long now = millis();
  int currentCount = encoder.getCount();
  if (currentCount % 2 == 0 && currentCount != debouncedCount && (now - lastEncoderUpdate >= ENCODER_DEBOUNCE_MS)) {
    debouncedCount = (currentCount % 2 == 0 ? currentCount : currentCount - 1) / 2;
    lastEncoderUpdate = now;
  }
}

void updateUI() {
  // Only refresh timer numbers
  int remaining = timer.getRemainingSeconds();
  int minutes = remaining / 60;
  int seconds = remaining % 60;
  drawTimer(tft, minutes, seconds);
}

#ifndef UNIT_TEST
void setup() {
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight ON
  Serial.begin(115200);
  delay(100);
  Serial.println("Booting... (Serial OK)");
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  tft.begin();
  tft.setRotation(1);
  // Remove EN_BUTTON logic: do not set up GPIO0 for mode toggle
  // pinMode(EN_BUTTON, INPUT_PULLUP); // <-- removed
  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DT, INPUT);
  pinMode(ROTARY_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ROTARY_SW), handleRotaryButton, FALLING); // Ensure interrupt is set
  // Encoder is global; do not reassign
  ESP32Encoder::useInternalWeakPullResistors = puType::none;
  encoder.attachHalfQuad(ROTARY_DT, ROTARY_CLK);
  encoder.setFilter(1023);
  encoder.clearCount();
  debouncedCount = 0;
  lastCount = 0;
  settings.load();
  // Re-init timer with persisted settings
  timer = TimerController(settings.focusSeconds(), settings.breakSeconds());
  buzzerSetup();
  tft.fillScreen(COLOR_BG); 
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
  // Button press/release logic
  static bool deepSleepArmed = false;
  static bool longPressHandled = false;
  if (rotaryButtonDown) {
    if (digitalRead(ROTARY_SW) == LOW) {
      // Button is still held
      if (!longPressHandled && millis() - rotaryButtonDownMs > ROTARY_BUTTON_LONGPRESS_MS) {
        longPressHandled = true;
        deepSleepArmed = true;
        Serial.println("Long press detected: entering deep sleep...");
        delay(100);
        // Wait for button release to avoid instant wakeup
        while (digitalRead(ROTARY_SW) == LOW) {
          delay(10);
        }
        // Prepare wakeup on GPIO32 (rotary button)
        esp_sleep_enable_ext0_wakeup((gpio_num_t)ROTARY_SW, 0); // Wake on LOW
  tft.fillScreen(TFT_BLACK);
  digitalWrite(TFT_BACKLIGHT, LOW); // Backlight OFF
  delay(100);
  esp_deep_sleep_start();
      }
    } else {
      // Button released
      if (!longPressHandled) {
        // Short press: treat as click
        rotaryButtonPressed = true;
      }
      rotaryButtonDown = false;
      longPressHandled = false;
      deepSleepArmed = false;
    }
  }
  static unsigned long lastButtonDebug = 0;
  if (millis() - lastButtonDebug > 500) {
    int swState = digitalRead(ROTARY_SW);
    Serial.print("ROTARY_SW (GPIO32) state: ");
    Serial.println(swState == LOW ? "LOW (pressed)" : "HIGH (released)");
    lastButtonDebug = millis();
  }
  // Rotary encoder button handling (toggle mode)
  if (rotaryButtonPressed) {
    rotaryButtonPressed = false;
    Serial.println("Rotary button pressed!");
    // Same as manual toggle
    timer.toggleMode();
    currentMode = timer.isFocus() ? FOCUS : BREAK;
    tft.fillScreen(COLOR_BG);
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
    if (currentMode == FOCUS) {
      buzzerBeep(150, BUZZER_DUTY_PERCENT);
    } else {
      buzzerDoubleBeep(120, 120, 120, BUZZER_DUTY_PERCENT);
    }
  }
  // Rotary encoder rotation debug
  if (debouncedCount != lastCount) {
    Serial.print("Rotary encoder count: ");
    Serial.println(debouncedCount);
    // For now, just toggle mode on any rotation
    timer.toggleMode();
    currentMode = timer.isFocus() ? FOCUS : BREAK;
    tft.fillScreen(COLOR_BG);
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
    if (currentMode == FOCUS) {
      buzzerBeep(150, BUZZER_DUTY_PERCENT);
    } else {
      buzzerDoubleBeep(120, 120, 120, BUZZER_DUTY_PERCENT);
    }
    lastCount = debouncedCount;
  }
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
  updateProgressAnimation(tft, 0, 0, PROGRESS_DIAMETER);
    #endif
  #endif
  // Non-blocking buzzer service
  buzzerService();
}
#endif // UNIT_TEST
