#ifndef ANNIVERSARY_H
#define ANNIVERSARY_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>
#include "timer.h"
#include "gfx_utils.h"
#include "icons.h"
#include "icon.h"
#include "menu.h"
#include "debug.h"
#include "button.h"
#include <vector>

class Anniversary
{
private:
    DISPLAY_CLASS &display;

    Menu buttons = Menu(display, new MenuItem[1]{MenuItem("Weiter")}, 1);

    int page = 0;
    const uint16_t numPages = 17;
    unsigned long lastRedrawTime = 0;

    void drawPage(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

public:
    Anniversary(DISPLAY_CLASS &display);
    ~Anniversary();
    void draw();
    void loop();
};

#endif
