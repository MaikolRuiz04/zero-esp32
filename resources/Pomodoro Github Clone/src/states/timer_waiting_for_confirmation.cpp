#include "../timer.h"

void Timer::handleWaitingForConfirmation(volatile int *encoderCount)
{
    elapsed = millis() - startTime - totalPausedTime;

    const int encoderDelta = *encoderCount - lastEncoderCount;
    if (encoderDelta != 0)
    {
        if (encoderDelta > 0)
        {
            confirmationMenu->next();
        }
        else
        {
            confirmationMenu->previous();
        }
        needsRedraw = true;
        lastEncoderCount = *encoderCount;
    }

    if (Button::instance->checkAndClearButtonPress())
    {
        if (state == TimerState::WaitingConfirmStartOfBreak)
        {
            Serial.println("Timer::handleWaitingForConfirmation: confirmed start of break");
            messageCache.clearCache(Messages::TimerWaitingForConfirmationStartOfBreak_Header);
            startBreak();
            needsFullRedraw = true;
        }
        else
        {
            Serial.println("Timer::handleWaitingForConfirmation: handling end of break");
            messageCache.clearCache(Messages::TimerWaitingForConfirmationEndOfBreak_Header);

            incrementTotalBreakTime(elapsed);
            minutesOnBreak += elapsed / 1000 / 60;

            // Handle menu selection
            if (confirmationMenu->getSelectedIndex() == 0)
            {
                // Restart timer
                start();
            }
            else
            {
                // Back to presets
                reset();
                enterPresetSelection();
            }
            needsFullRedraw = true;
        }
    }

    if (millis() - lastRedrawTime >= redrawInterval)
    {
        flashingIcon = !flashingIcon;
        needsRedraw = true;
    }
}

void Timer::drawWaitingForConfirmation()
{
    // const auto foreground = flashingIcon ? GxEPD_BLACK : GxEPD_WHITE;
    // const auto background = flashingIcon ? GxEPD_WHITE : GxEPD_BLACK;
    const auto foreground = GxEPD_BLACK;
    const auto background = GxEPD_WHITE;

    display.fillScreen(background);

    ScaledIcon icon = icon_lpetantrum.scaled(128);

    const char *text = state == TimerState::WaitingConfirmStartOfBreak ? messageCache.getMessage(Messages::TimerWaitingForConfirmationStartOfBreak_Header) : messageCache.getMessage(Messages::TimerWaitingForConfirmationEndOfBreak_Header);
    Bounds textBounds = getBounds(display, text, &SEMI_LARGE_FONT);

    const int16_t padding = 20;
    const auto boxWidth = padding + icon.size + padding + textBounds.w + padding;
    const auto boxHeight = icon.size + padding * 2;
    const auto boxX = display.width() / 2 - boxWidth / 2;
    const auto boxY = display.height() / 2 - boxHeight / 2;

    // Draw main box with icon and text
    display.drawRoundRect(boxX, boxY, boxWidth, boxHeight, 10, foreground);

    // Draw icon on the left
    display.drawBitmap(boxX + padding, boxY + padding, icon.data, icon.size, icon.size, foreground);

    // Draw main text vertically centered with icon
    drawText(display, text,
             boxX + padding + icon.size + padding,
             boxY + boxHeight / 2 + textBounds.h / 2,
             &SEMI_LARGE_FONT, foreground);

    // Draw menu options - either single item or two items based on state
    const uint16_t menuY = boxY + boxHeight + padding;
    const uint16_t paddingBetweenBoxes = 20;
    const uint16_t menuItemCount = (state == TimerState::WaitingConfirmStartOfBreak) ? 1 : 2;
    const uint16_t menuItemWidth = (state == TimerState::WaitingConfirmStartOfBreak) ? boxWidth : (boxWidth - paddingBetweenBoxes) / 2;
    const uint16_t menuHeight = 48;

    for (int i = 0; i < menuItemCount; i++)
    {
        const bool selected = i == confirmationMenu->getSelectedIndex();
        auto item = confirmationMenu->getItems()[i];
        const uint16_t xOffset = (state == TimerState::WaitingConfirmStartOfBreak) ? boxX : boxX + i * (menuItemWidth + paddingBetweenBoxes);

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
}