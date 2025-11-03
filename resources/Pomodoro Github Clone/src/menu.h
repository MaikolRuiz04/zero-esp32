#ifndef MENU_H
#define MENU_H

#include "defs.h"
#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>
#include "icon.h"
#include "debug.h"

enum class DrawStyle
{
    Vertical,
    Horizontal
};

class MenuItem
{
private:
    const char *text;
    Icon *icon;

public:
    MenuItem(const char *text, Icon *icon = nullptr);
    ~MenuItem();

    const char *getText();
    void setText(const char *text);
    Icon *getIcon();
};

class Menu
{
private:
    DISPLAY_CLASS &display; // Update display type
    MenuItem *items;
    int itemCount;
    int selectedIndex;

    int lastEncoderCount = 0;

public:
    Menu(DISPLAY_CLASS &display, MenuItem *items, int itemCount); // Update constructor
    ~Menu();

    MenuItem *getSelected();
    MenuItem *getItems();
    int getSelectedIndex();
    int getItemCount();

    void setSelectedIndex(int index);
    void setEncoderCount(int encoderCount);

    void next();
    void previous();

    bool loop(volatile int *encoderCount);
};

#endif // MENU_H
