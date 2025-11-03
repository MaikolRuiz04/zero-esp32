#include "menu.h"

Menu::Menu(DISPLAY_CLASS &display, MenuItem *items, int itemCount) : display(display) // Update constructor
{
    this->items = items;
    this->itemCount = itemCount;
    this->selectedIndex = 0;
}

Menu::~Menu()
{
}

MenuItem *Menu::getSelected()
{
    return &items[selectedIndex];
}

MenuItem *Menu::getItems()
{
    return items;
}

int Menu::getSelectedIndex()
{
    return selectedIndex;
}

int Menu::getItemCount()
{
    return itemCount;
}

void Menu::setSelectedIndex(int index)
{
    selectedIndex = index;
}

void Menu::setEncoderCount(int encoderCount)
{
    lastEncoderCount = encoderCount;
}

void Menu::next()
{
    selectedIndex = (selectedIndex + 1) % itemCount;
}

void Menu::previous()
{
    selectedIndex = (selectedIndex - 1 + itemCount) % itemCount;
}

bool Menu::loop(volatile int *encoderCount)
{
    if (*encoderCount != lastEncoderCount)
    {
        if (*encoderCount < lastEncoderCount)
        {
            previous();
        }
        else
        {
            next();
        }

        lastEncoderCount = *encoderCount;
        return true;
    }

    return false;
}

MenuItem::MenuItem(const char *text, Icon *icon) : text(text), icon(icon)
{
}

MenuItem::~MenuItem()
{
}

const char *MenuItem::getText()
{
    return text;
}

void MenuItem::setText(const char *text)
{
    this->text = text;
}

Icon *MenuItem::getIcon()
{
    return icon;
}
