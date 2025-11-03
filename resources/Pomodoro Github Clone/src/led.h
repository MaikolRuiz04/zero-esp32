#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>

enum class LedMode
{
    Off,
    Splashscreen,
    QuickAcknowledgementFlash,
    ConfirmationFlash,
    TimerPaused,
};

void setupLed();
void ledSetupEncoder(volatile int *encoderCount);
void stopLed();
void startLedTask();
void setLedMode(LedMode mode);

extern TaskHandle_t ledTaskHandle;

#endif