#pragma once
#include <Arduino.h>

// Rotary encoder (incremental, 2-channel + push switch) abstraction.
// Provides debounced event polling without interrupts.
// Design notes:
//  - We only emit a rotation event on the FALLING edge of CLK to avoid double
//    ticks (KY-040 produces two transitions per detent).
//  - Press detection requires a stable LOW after the initial falling edge
//    (non-blocking). This reduces accidental mode commits caused by electrical
//    noise while rotating.
//  - Direction heuristic: if DT != CLK at the moment of the CLK falling edge -> clockwise (+1), else counter‑clockwise (-1).
// Usage:
//   encoderInit(clkPin, dtPin, swPin, stepDebounceMs, pressDebounceMs);
//   EncoderEvent ev = encoderPoll();
//   if (ev.type == ENCODER_ROTATE) { /* ev.direction = +1/-1 */ }
//   if (ev.type == ENCODER_PRESS)  { /* handle click */ }

enum EncoderEventType { ENCODER_NONE = 0, ENCODER_ROTATE, ENCODER_PRESS };

struct EncoderEvent {
    EncoderEventType type;
    int direction; // +1 clockwise, -1 counter‑clockwise, 0 for press
};

void encoderInit(int pinClk, int pinDt, int pinSw,
                 unsigned long stepDebounceMs = 2,
                 unsigned long pressDebounceMs = 180,
                 unsigned long pressStableMs = 30);

// Poll frequently (each loop). Returns a single event (if any) per call.
EncoderEvent encoderPoll();

// Optional utility: if you need to query hold state later.
bool encoderSwitchIsDown();
// Returns majority-filtered instantaneous stable switch state (active LOW)
bool encoderSwitchStable();

// Optional (compile-time) debug – define ENCODER_DEBUG before including header
#ifndef ENCODER_DEBUG
#define ENCODER_DEBUG 0
#endif
