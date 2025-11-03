#include "splashscreen.h"
#include "button.h"
#include "icon_provider.h"

SplashScreen::SplashScreen(DISPLAY_CLASS &display, Timer &timer) : display(display), timer(timer)
{
    for (int i = 0; i < checkboxes.size(); i++)
    {
        checkboxes[i].load();
    }

    selectedSettingsIndex = 0;
    selectedCheckbox = &checkboxes[selectedSettingsIndex];
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::draw()
{
    display.fillScreen(GxEPD_WHITE);

    display.drawBitmap(0, 0, image_bg_splash, display.width(), display.height(), GxEPD_BLACK);

    const uint16_t padding = 20;
    const uint16_t menuY = padding;
    const uint16_t paddingBetweenBoxes = 20;
    const uint16_t menuItemCount = buttons.getItemCount();
    const uint16_t menuItemWidth = (display.width() - paddingBetweenBoxes * (menuItemCount - 1) - padding * 2) / menuItemCount;
    const uint16_t menuHeight = 48;

    for (int i = 0; i < menuItemCount; i++)
    {
        const bool selected = i == buttons.getSelectedIndex();
        auto item = buttons.getItems()[i];
        const uint16_t xOffset = padding + i * (menuItemWidth + paddingBetweenBoxes);

        if (selected)
        {
            display.fillRoundRect(xOffset, menuY, menuItemWidth, menuHeight, 10, GxEPD_WHITE);
            drawPatternInRoundedArea(display, xOffset, menuY, menuItemWidth, menuHeight, 10, Pattern::SparseDots);
        }
        else
        {
            display.fillRoundRect(xOffset, menuY, menuItemWidth, menuHeight, 10, GxEPD_WHITE);
        }
        display.drawRoundRect(xOffset, menuY, menuItemWidth, menuHeight, 10, GxEPD_BLACK);
        drawCenteredText(display, item.getText(), xOffset + menuItemWidth / 2, menuY + menuHeight / 2, &SUB_FONT, GxEPD_BLACK);
    }

    display.display(true);
}

void SplashScreen::loop(volatile int *encoderCount)
{
    while (true)
    {
        if (
            buttons.loop(encoderCount))
        {
            // Redraw
            draw();
            lastEncoderCount = *encoderCount;
        }

        if (!Button::instance)
        {
            Serial.println("Button instance is null");
            continue;
        }

        if (Button::instance->checkAndClearButtonPress())
        {
            if (buttons.getSelectedIndex() == 0)
            {
                timer.start();
                return;
            }
            else if (buttons.getSelectedIndex() == 1)
            {
                display.fillScreen(GxEPD_WHITE);
                drawSettings();
                loopSettings(encoderCount);
            }
            return;
        }
    }
}

void SplashScreen::drawSettings()
{
    display.fillScreen(GxEPD_WHITE);

    for (int i = 0; i < checkboxes.size(); i++)
    {
        auto checkbox = checkboxes[i];
        const bool selected = checkbox.getName() == selectedCheckbox->getName();

        checkbox.draw(display, 16, i * 96 + 16 + i * 32, display.width() - 32, 96, selected);
    }

    display.display(true);
}

void SplashScreen::loopSettings(volatile int *encoderCount)
{
    while (true)
    {
        if (lastEncoderCount != *encoderCount)
        {
            // select next or previous checkbox
            if (*encoderCount < lastEncoderCount)
            {
                selectedSettingsIndex = (selectedSettingsIndex - 1 + checkboxes.size()) % checkboxes.size();
                selectedCheckbox = &checkboxes[selectedSettingsIndex];
            }
            else
            {
                selectedSettingsIndex = (selectedSettingsIndex + 1) % checkboxes.size();
                selectedCheckbox = &checkboxes[selectedSettingsIndex];
            }

            // Redraw
            drawSettings();

            lastEncoderCount = *encoderCount;
        }

        if (Button::instance->checkAndClearButtonPress())
        {
            if (selectedCheckbox->getName() == "Reset Device")
            {
                Preferences preferences;
                preferences.begin("pomodoro", false);
                preferences.clear();
                preferences.end();

                display.fillScreen(GxEPD_BLACK);
                display.display();

                delay(5000);
                ESP.restart();
            }

            // save settings
            selectedCheckbox->toggle();
            selectedCheckbox->save();

            // Redraw
            drawSettings();

            delay(1000);

            ESP.restart();
        }
    }
}