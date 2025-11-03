#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button
{
private:
    int pin;
    static bool instanceExists;

public:
    volatile bool pressed = false;
    volatile unsigned long lastPressTime = 0;

    static Button *instance;

    Button(int pin);
    ~Button();

    static void IRAM_ATTR buttonInterruptHandler();
    bool checkAndClearButtonPress();
};
#endif