#include "button.h"

static const unsigned long DEBOUNCE_DELAY = 1000; // ms

Button *Button::instance = nullptr;
bool Button::instanceExists = false;

Button::Button(int pin) : pin(pin)
{
    if (instanceExists)
    {
        Serial.println("ERROR: Only one Button instance allowed!");
        return;
    }

    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(pin, buttonInterruptHandler, FALLING);
    instance = this;
    instanceExists = true;
}

Button::~Button()
{
    if (instance == this)
    {
        instance = nullptr;
        instanceExists = false;
        detachInterrupt(pin);
    }
}

void IRAM_ATTR Button::buttonInterruptHandler()
{
    if (!instance)
        return; // Safety check

    unsigned long currentTime = millis();

    if ((currentTime - instance->lastPressTime) > DEBOUNCE_DELAY)
    {
        instance->pressed = true;
        instance->lastPressTime = currentTime;
    }
}

bool Button::checkAndClearButtonPress()
{
    if (!instanceExists)
        return false; // Safety check

    if (pressed)
    {
        Serial.println("Button pressed");
        pressed = false;
        return true;
    }
    return false;
}