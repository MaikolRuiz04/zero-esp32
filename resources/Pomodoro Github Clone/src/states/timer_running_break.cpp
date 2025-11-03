#include "../timer.h"
#include "../strings.h"

void Timer::handleRunningBreak(volatile int *encoderCount)
{
    if (state == TimerState::RunningBreak)
    {
        elapsed = millis() - startTime - totalPausedTime;
    }

    if (state == TimerState::RunningBreak && millis() - startTime >= currentBreakDuration)
    {
        setLedMode(LedMode::ConfirmationFlash);
        state = TimerState::WaitingConfirmEndOfBreak;

        this->confirmationMenu = new Menu(display, new MenuItem[2]{MenuItem(messageCache.getMessage(Messages::MenuItem_RestartTimer)), MenuItem(messageCache.getMessage(Messages::MenuItem_BackToPresets))}, 2);
        this->confirmationMenu->setEncoderCount(*encoderCount); // Sync encoder count
        needsFullRedraw = true;
        return;
    }

    // Handle menu input
    if (topMenu)
    {
        if (Button::instance->checkAndClearButtonPress())
        {
            // Handle menu selection
            if (topMenu->getSelectedIndex() == 0)
            {
                if (state == TimerState::RunningBreak)
                {
                    // Pause
                    pause();
                }
                else
                {
                    // Resume
                    resume();
                }
            }
            else if (topMenu->getSelectedIndex() == 1)
            {
                // Skip break
                incrementTotalBreakTime(elapsed);
                minutesOnBreak += elapsed / 1000 / 60;
                reset();
                enterPresetSelection();
            }
            else if (topMenu->getSelectedIndex() == 2)
            {
                // Cancel
                incrementTotalBreakTime(elapsed);
                minutesOnBreak += elapsed / 1000 / 60;
                stop();
                selectPreset(1);
                enterPresetSelection();
            }

            needsFullRedraw = true;
        }

        if (topMenu->loop(encoderCount))
        {
            menuNeedsRedraw = true;
        }
    }

    if (millis() - lastRedrawTime >= redrawInterval)
    {
        needsRedraw = true;
    }
}

String formatDuration(unsigned long minutes)
{
    unsigned long hours = minutes / 60;
    minutes = minutes % 60;

    char buffer[32];
    if (hours > 0)
    {
        sprintf(buffer, "%luh %lum", hours, minutes);
    }
    else
    {
        sprintf(buffer, "%lum", minutes);
    }

    return String(buffer);
}

void Timer::drawRunningBreak()
{
    display.fillScreen(GxEPD_WHITE);

    if (isLongBreak)
    {
        display.drawBitmap(0, 8 + 48 + 4 + 8, breakImage, display.width(), display.height() - 8 - 48 - 4 - 8 - 64 - 4, GxEPD_BLACK);
    }
    else
    {
        const uint16_t w = 420;
        const uint16_t h = 340;
        Bounds boxBounds = {display.width() / 2 - w / 2, display.height() - h - 64, w, h};
        const uint16_t innerPadding = 8;
        uint16_t yOffset = boxBounds.y + innerPadding;
        Bounds statistics = getBounds(display, messageCache.getMessage(Messages::Statistics), &MAIN_FONT);

        display.drawRoundRect(boxBounds.x, boxBounds.y, boxBounds.w, boxBounds.h, 10, GxEPD_BLACK);

        yOffset += statistics.h;
        drawText(display, "Statistik", boxBounds.x + innerPadding, yOffset, &MAIN_FONT, GxEPD_BLACK);

        yOffset += innerPadding;
        drawPattern(display, Pattern::SparseDots, boxBounds.x, boxBounds.y, boxBounds.w, yOffset - boxBounds.y);
        display.drawFastHLine(boxBounds.x, yOffset, boxBounds.w, GxEPD_BLACK);

        // fetch statistics
        unsigned int totalCycles;
        unsigned long totalTime, totalBreakTime;
        getStatistics(&totalCycles, &totalTime, &totalBreakTime);

        yOffset += 1 + innerPadding;

        Bounds textBounds;

        /// Current session

        textBounds = drawBottomAlignedText(display, messageCache.getMessage(Messages::Statistics_CurrentCycle), boxBounds.x + innerPadding, yOffset, &SUB_FONT, GxEPD_BLACK);
        Bounds boundsCurrentCycles = getBounds(display, String(cycles).c_str(), &SUB_FONT);
        drawBottomAlignedText(display, String(cycles).c_str(), boxBounds.x + boxBounds.w - innerPadding - boundsCurrentCycles.w, yOffset, &SUB_FONT, GxEPD_BLACK);

        yOffset += textBounds.h + innerPadding;

        textBounds = drawBottomAlignedText(display, messageCache.getMessage(Messages::Statistics_CurrentTime), boxBounds.x + innerPadding, yOffset, &SUB_FONT, GxEPD_BLACK);
        Bounds boundsCurrentTime = getBounds(display, formatDuration(minutesWorked).c_str(), &SUB_FONT);
        drawBottomAlignedText(display, formatDuration(minutesWorked).c_str(), boxBounds.x + boxBounds.w - innerPadding - boundsCurrentTime.w, yOffset, &SUB_FONT, GxEPD_BLACK);

        yOffset += textBounds.h + innerPadding;

        textBounds = drawBottomAlignedText(display, messageCache.getMessage(Messages::Statistics_CurrentBreakTime), boxBounds.x + innerPadding, yOffset, &SUB_FONT, GxEPD_BLACK);
        Bounds boundsCurrentBreakTime = getBounds(display, formatDuration(minutesOnBreak).c_str(), &SUB_FONT);
        drawBottomAlignedText(display, formatDuration(minutesOnBreak).c_str(), boxBounds.x + boxBounds.w - innerPadding - boundsCurrentBreakTime.w, yOffset, &SUB_FONT, GxEPD_BLACK);

        yOffset += textBounds.h + innerPadding;

        // Divider

        yOffset += 2 * innerPadding;

        textBounds = drawBottomAlignedText(display, "Gesamt", boxBounds.x + innerPadding, yOffset + innerPadding, &SUB_FONT, GxEPD_BLACK);
        drawPattern(display, Pattern::Dots, boxBounds.x, yOffset, boxBounds.w, textBounds.h + 2 * innerPadding);

        yOffset += textBounds.h + 2 * innerPadding + innerPadding;

        // All time stats

        textBounds = drawBottomAlignedText(display, messageCache.getMessage(Messages::Statistics_TotalCycles), boxBounds.x + innerPadding, yOffset, &SUB_FONT, GxEPD_BLACK);

        Bounds boundsTotalCycles = getBounds(display, String(totalCycles).c_str(), &SUB_FONT);
        drawBottomAlignedText(display, String(totalCycles).c_str(), boxBounds.x + boxBounds.w - innerPadding - boundsTotalCycles.w, yOffset, &SUB_FONT, GxEPD_BLACK);

        yOffset += textBounds.h + innerPadding;

        textBounds = drawBottomAlignedText(display, messageCache.getMessage(Messages::Statistics_TotalTime), boxBounds.x + innerPadding, yOffset, &SUB_FONT, GxEPD_BLACK);

        auto totalTimeStr = formatDuration(totalTime);
        Bounds boundsTotalTime = getBounds(display, String(totalTimeStr).c_str(), &SUB_FONT);
        drawBottomAlignedText(display, String(totalTimeStr).c_str(), boxBounds.x + boxBounds.w - innerPadding - boundsTotalTime.w, yOffset, &SUB_FONT, GxEPD_BLACK);

        yOffset += textBounds.h + innerPadding;

        textBounds = drawBottomAlignedText(display, messageCache.getMessage(Messages::Statistics_TotalBreakTime), boxBounds.x + innerPadding, yOffset, &SUB_FONT, GxEPD_BLACK);

        auto totalBreakTimeStr = formatDuration(totalBreakTime);
        Bounds boundsTotalBreakTime = getBounds(display, String(totalBreakTimeStr).c_str(), &SUB_FONT);
        drawBottomAlignedText(display, String(totalBreakTimeStr).c_str(), boxBounds.x + boxBounds.w - innerPadding - boundsTotalBreakTime.w, yOffset, &SUB_FONT, GxEPD_BLACK);

        yOffset += textBounds.h + innerPadding;
    }

    drawMenuBar();

    unsigned int remainingUnit = 0;
    char buffer[64]; // Increased buffer size to be safe

    const unsigned int remainingMillis = currentBreakDuration - elapsed;
    const unsigned int seconds = remainingMillis / 1000;
    const unsigned int minutes = max(seconds / 60, 1u);
    uint16_t roundedSeconds = (seconds + 9) / 10 * 10;

    if (roundedSeconds >= 60)
    {
        sprintf(buffer, "%s - %d %s", messageCache.getMessage(isLongBreak ? Messages::Break_LongPauseText : Messages::Break_PauseText), minutes, messageCache.getMessage(Messages::TimeFormat_Minutes));
    }
    else
    {
        if (redrawInterval != REDRAW_INTERVAL_FAST)
        {
            redrawInterval = REDRAW_INTERVAL_FAST;
            needsRedraw = true;
        }

        // Round to nearest 10 seconds when below 1 minute
        sprintf(buffer, "%s - %d %s", messageCache.getMessage(isLongBreak ? Messages::Break_LongPauseText : Messages::Break_PauseText), roundedSeconds, messageCache.getMessage(Messages::TimeFormat_Seconds));
    }

    Bounds boundsMin = getBounds(display, buffer, &MAIN_FONT);

    // Draw text in the center
    drawText(display, buffer, display.width() / 2 - boundsMin.w / 2, display.height() - 64 / 2 + boundsMin.h / 2, &MAIN_FONT, GxEPD_BLACK);
}