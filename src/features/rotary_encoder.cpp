#include "features/rotary_encoder.h"
#include <Arduino.h>
#include "ui_design.h" // for ENCODER_CFG_* macros

static int s_clkPin = -1;
static int s_dtPin  = -1;
static int s_swPin  = -1;
static int s_lastClk = HIGH;
static int s_lastSw  = HIGH;
static unsigned long s_lastStepMs = 0;
static unsigned long s_lastPressMs = 0;      // last accepted press time
static unsigned long s_pressEdgeMs = 0;      // initial falling edge time
static bool s_pressCandidate = false;        // tracking potential press
static bool s_pressLatched = false;          // true after one press event until release
static unsigned long s_stepDebounceMs = 2;
static unsigned long s_pressDebounceMs = 180; // min gap between distinct presses
static unsigned long s_pressStableMs = 30;    // required stable LOW duration
static int s_lastDt = HIGH;
static bool s_pendingRotation = false;
static int s_pendingDirection = 0;
static unsigned long s_pendingEdgeMs = 0;
static unsigned long s_lastConfirmedRotationMs = 0;

void encoderInit(int pinClk, int pinDt, int pinSw,
                 unsigned long stepDebounceMs, unsigned long pressDebounceMs,
                 unsigned long pressStableMs) {
    s_clkPin = pinClk; s_dtPin = pinDt; s_swPin = pinSw;
    s_stepDebounceMs = stepDebounceMs; s_pressDebounceMs = pressDebounceMs; s_pressStableMs = pressStableMs;
    pinMode(s_clkPin, INPUT_PULLUP);
    pinMode(s_dtPin, INPUT_PULLUP);
    pinMode(s_swPin, INPUT_PULLUP);
    s_lastClk = digitalRead(s_clkPin);
    s_lastSw  = digitalRead(s_swPin);
    s_pressCandidate = false;
    s_pressLatched = false;
    s_lastDt = digitalRead(s_dtPin);
    s_pendingRotation = false;
    s_lastConfirmedRotationMs = 0;
}

EncoderEvent encoderPoll() {
    EncoderEvent ev{ENCODER_NONE, 0};
    unsigned long now = millis();

    // Rotation detection with optional stability stage
    int clkState = digitalRead(s_clkPin);
    if (s_lastClk == HIGH && clkState == LOW) { // falling edge
        if (now - s_lastStepMs >= s_stepDebounceMs) {
            if (ENCODER_CFG_SUPPRESS_ROTATE_DURING_PRESS && s_pressCandidate) {
#if ENCODER_DEBUG
                Serial.println("[ENC] rotation ignored (press candidate)");
#endif
            } else {
                int dtState = digitalRead(s_dtPin);
                int direction = (dtState != clkState) ? +1 : -1; // dt != low -> +1
                if (ENCODER_CFG_REQUIRE_STABLE_DT && ENCODER_CFG_ROTATE_STABILITY_MS > 0) {
                    s_pendingRotation = true;
                    s_pendingDirection = direction;
                    s_pendingEdgeMs = now;
                } else {
                    ev.type = ENCODER_ROTATE;
                    ev.direction = direction;
                    s_lastStepMs = now;
                    s_lastConfirmedRotationMs = now;
                }
#if ENCODER_DEBUG
                Serial.print("[ENC] ROT edge dir="); Serial.println(direction);
#endif
            }
        }
    }
    s_lastClk = clkState;
    // Confirm pending rotation after stability window
    if (s_pendingRotation && (now - s_pendingEdgeMs) >= ENCODER_CFG_ROTATE_STABILITY_MS) {
        int dtNow = digitalRead(s_dtPin);
        if (dtNow == s_lastDt) {
            ev.type = ENCODER_ROTATE;
            ev.direction = s_pendingDirection;
            s_lastStepMs = now;
            s_lastConfirmedRotationMs = now;
#if ENCODER_DEBUG
            Serial.print("[ENC] ROT confirmed dir="); Serial.println(ev.direction);
#endif
        } else {
#if ENCODER_DEBUG
            Serial.println("[ENC] ROT cancelled (dt changed)");
#endif
        }
        s_pendingRotation = false;
    }
    s_lastDt = digitalRead(s_dtPin);
    if (ev.type != ENCODER_NONE) return ev; // rotation event wins

    // Switch press detection (active LOW, needs stable low)
    // Majority filter (5 rapid samples) to mitigate floating / bounce artifacts
    int lowCount = 0;
    int sample;
    for (int i = 0; i < 5; ++i) {
        sample = digitalRead(s_swPin);
        if (sample == LOW) lowCount++;
    }
    int swState = (lowCount >= 3) ? LOW : HIGH;
#if ENCODER_DEBUG && 0
    Serial.print("[ENC] SW samples low="); Serial.println(lowCount);
#endif
    if (!s_pressCandidate) {
        // Initial falling edge
        if (swState == LOW && s_lastSw == HIGH) {
            if (now - s_lastConfirmedRotationMs < 220) {
#if ENCODER_DEBUG
                Serial.println("[ENC] press suppressed (recent rotate)");
#endif
            } else {
                s_pressEdgeMs = now;
                s_pressCandidate = true;
                s_pressLatched = false;
            }
        }
    } else {
        // Candidate in progress
        if (swState == HIGH) {
            // Bounce ended / cancelled
            s_pressCandidate = false;
            s_pressLatched = false; // allow next press
        } else if (now - s_pressEdgeMs >= s_pressStableMs) {
            // Stable LOW long enough – treat as press ONCE per hold
            if (!s_pressLatched && (now - s_lastPressMs >= s_pressDebounceMs)) {
                ev.type = ENCODER_PRESS; // fire on hold qualification
                ev.direction = 0;
                s_lastPressMs = now;
                s_pressLatched = true; // block repeats until release
#if ENCODER_DEBUG
                Serial.println("[ENC] PRESS");
#endif
            }
            // Wait for release before another press
        }
    }
    s_lastSw = swState;
    return ev;
}

bool encoderSwitchStable() {
    int lowCount = 0;
    for (int i = 0; i < 5; ++i) {
        if (digitalRead(s_swPin) == LOW) lowCount++;
    }
    return lowCount >= 3;
}

bool encoderSwitchIsDown() { return encoderSwitchStable(); }
