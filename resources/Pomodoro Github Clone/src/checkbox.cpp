#include "checkbox.h"
#include "preferences_manager.h"

extern Preferences preferences;

#define NOVALUE 0
#define TRUEVALUE -1
#define FALSEVALUE 1

Checkbox::Checkbox(Icon *icon, const char *name, const char *key, bool defaultValue) : icon(icon), name(name), key(key), defaultValue(defaultValue)
{
    load();
}

Checkbox::~Checkbox()
{
    save();
}

Icon *Checkbox::getIcon()
{
    return icon;
}

const char *Checkbox::getName()
{
    return name;
}

bool Checkbox::isChecked()
{
    return checked;
}

void Checkbox::toggle()
{
    checked = !checked;
}

void Checkbox::load()
{
    checked = pref_getCheckbox(key, defaultValue);

    Serial.printf("Checkbox::load: key=%s, value=%s\n", key, checked ? "true" : "false");
}

void Checkbox::save()
{
    pref_putCheckbox(key, checked);

    Serial.printf("Checkbox::save: key=%s, value=%s\n", key, checked ? "true" : "false");
}

void Checkbox::draw(
    DISPLAY_CLASS &display,
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    bool selected)
{
    const uint16_t padding = 12;

    display.fillRect(x, y, w, h, GxEPD_WHITE);
    display.drawRoundRect(x, y, w, h, 10, GxEPD_BLACK);

    if (selected)
    {
        drawPatternInRoundedArea(display, x, y, w, h, 10, Pattern::SparseDots);
    }

    ScaledIcon checkmark = icon_checkmark.scaled(48);

    const uint16_t iconSize = 64;
    if (icon)
    {
        ScaledIcon scaledIcon = icon->scaled(iconSize);
        display.drawBitmap(x + padding, y + padding, scaledIcon.data, scaledIcon.size, scaledIcon.size, GxEPD_BLACK);
    }

    Bounds bounds = getBounds(display, name, &MAIN_FONT);
    const uint16_t textX = x + padding + iconSize + padding;
    const uint16_t textY = y + h / 2 + bounds.h / 2;

    drawText(display, name, textX, textY, &MAIN_FONT, GxEPD_BLACK);

    display.drawRoundRect(x + w - padding - 64, y + h / 2 - 64 / 2, 64, 64, 10, GxEPD_BLACK);

    if (checked)
    {
        display.drawBitmap(x + w - padding - 64 / 2 - checkmark.size / 2, y + h / 2 - checkmark.size / 2, checkmark.data, checkmark.size, checkmark.size, GxEPD_BLACK);
    }
}