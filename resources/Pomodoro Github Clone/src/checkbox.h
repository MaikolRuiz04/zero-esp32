#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <Arduino.h>
#include <Preferences.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>
#include "icons.h"
#include "icon.h"
#include "gfx_utils.h"
#include "debug.h"
#include "defs.h"

class Checkbox
{
private:
    Icon *icon;
    const char *name;
    const char *key;
    bool checked;
    bool defaultValue;

    Preferences preferences;

public:
    Checkbox(Icon *icon, const char *name, const char *key, bool defaultValue = false);
    ~Checkbox();
    Icon *getIcon();
    const char *getName();
    bool isChecked();
    void toggle();

    void load();
    void save();

    void draw(
        DISPLAY_CLASS &display,
        uint16_t x,
        uint16_t y,
        uint16_t w,
        uint16_t h,
        bool selected);
};

#endif