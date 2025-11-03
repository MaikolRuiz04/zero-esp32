#include "../timer.h"
#include "../strings.h"
#include <sstream>

void Timer::handleRunning(volatile int *encoderCount)
{
    if (state == TimerState::Running)
    {
        elapsed = millis() - startTime - totalPausedTime;
    }

    if (state == TimerState::Running && millis() - lastMessageUpdate >= RUNNING_MESSAGE_REFRESH_INTERVAL)
    {
        lastMessageUpdate = millis();
        messageCache.clearCache(Messages::Preset_Email_Message);
        messageCache.clearCache(Messages::Preset_Coding_Message);
        messageCache.clearCache(Messages::Preset_Focus_Message);

        needsRedraw = true;
    }

    if (state == TimerState::Running && elapsed >= currentPreset->getDuration())
    {

        longestEarnedPauseInShortCycles = max(currentPreset->getLongPauseDuration(), longestEarnedPauseInShortCycles);

        setLedMode(LedMode::ConfirmationFlash);
        state = TimerState::WaitingConfirmStartOfBreak;

        this->confirmationMenu = new Menu(display, new MenuItem[1]{MenuItem(messageCache.getMessage(Messages::MenuItem_StartBreak))}, 1);
        this->confirmationMenu->setEncoderCount(*encoderCount); // Sync the encoder count
        needsFullRedraw = true;
        return;
    }

    // Only trigger redraw once per second to avoid unnecessary updates
    if (millis() - lastRedrawTime >= redrawInterval)
    {
        needsRedraw = true;
    }

    // Handle menu input
    if (topMenu)
    {
        if (Button::instance->checkAndClearButtonPress())
        {
            // Handle menu selection
            if (topMenu->getSelectedIndex() == 0)
            {
                if (state == TimerState::Running)
                {
                    // Pause
                    pause();
                    topMenu->getSelected()->setText(messageCache.getMessage(Messages::MenuItem_Resume));
                }
                else
                {
                    // Resume
                    resume();
                    topMenu->getSelected()->setText(messageCache.getMessage(Messages::MenuItem_Pause));
                }

                needsRedraw = true;
            }
            else if (topMenu->getSelectedIndex() == 1)
            {
                // Break now
                startBreak();
            }
            else if (topMenu->getSelectedIndex() == 2)
            {
                // Cancel
                incrementTotalTime(elapsed);
                minutesWorked += elapsed / 1000 / 60;
                stop();
                enterPresetSelection();

                needsFullRedraw = true;
            }
        }

        if (topMenu->loop(encoderCount))
        {
            menuNeedsRedraw = true;
        }
    }
}

void Timer::drawRunning()
{
    display.fillScreen(GxEPD_WHITE);

    if (showSpeechBubble)
    {
        display.drawBitmap(0, 0, currentPreset->getBackground(), display.width(), display.height(), GxEPD_BLACK);
    }

    drawMenuBar();

    unsigned int remainingUnit = 0;
    char buffer[32]; // Increased buffer size to be safe

    const unsigned int remainingMillis = currentPreset->getDuration() - elapsed;
    const unsigned int seconds = remainingMillis / 1000;
    const unsigned int minutes = max(seconds / 60, 1u);
    uint16_t roundedSeconds = (seconds + 9) / 10 * 10;

    if (roundedSeconds >= 60)
    {
        sprintf(buffer, "%d %s", minutes, messageCache.getMessage(Messages::TimeFormat_Minutes));
    }
    else
    {
        if (redrawInterval != REDRAW_INTERVAL_FAST)
        {
            redrawInterval = REDRAW_INTERVAL_FAST;
            needsRedraw = true;
        }

        // Round to nearest 10 seconds when below 1 minute
        sprintf(buffer, "%d %s", roundedSeconds, messageCache.getMessage(Messages::TimeFormat_Seconds));
    }

    const uint16_t progressBarHeight = 32;

    Bounds boundsMin = getBounds(display, buffer, &LARGE_FONT);

    if (!showSpeechBubble)
    {
        // Draw text in the center
        drawText(display, buffer, display.width() / 2 - boundsMin.w / 2, display.height() / 2 + boundsMin.h / 2, &LARGE_FONT, GxEPD_BLACK);
        return;
    }

    drawText(display, buffer, display.width() / 2 - boundsMin.w / 2, display.height() / 3 + boundsMin.h / 2, &LARGE_FONT, GxEPD_BLACK);

    const unsigned int progress = (elapsed * 100) / currentPreset->getDuration();
    const uint16_t progressBarWidth = display.width() / 2;

    drawProgressBar(display, ProgressBarStyle::Bordered, display.width() / 2 - progressBarWidth / 2, display.height() / 3 + boundsMin.h / 2 + 16, progressBarWidth, progressBarHeight, progressBarHeight / 2, progress);

    drawDebugCrosshair(display, display.width() / 2, display.height() / 2, 48);

    // Space for message: 221,330 until 649,409
    const uint16_t messageMinX = 221;
    const uint16_t messageMaxX = 649;
    const uint16_t messageMinY = 330;
    const uint16_t messageMaxY = 409;
    const uint16_t messageW = messageMaxX - messageMinX;
    const uint16_t messageH = messageMaxY - messageMinY;

    drawDebugCrosshair(display, 221, 330);

    // Split message by lines and print individually
    std::string messageStr(getRunningMessage());
    std::istringstream iss(messageStr);
    std::string line;
    int lineIndex = 0;
    while (std::getline(iss, line, '\n'))
    {
        int yPos = messageMinY + lineIndex * 18 + 18 + lineIndex * 2; // + 18 because of the first line
        drawText(display, line.c_str(), messageMinX, yPos, &SMALL_FONT, GxEPD_BLACK);
        ++lineIndex;
    }
}

const char *Timer::getRunningMessage()
{
    if (currentPreset == nullptr)
    {
        return "No preset selected";
    }

    if (currentPreset->getName() == nullptr)
    {
        return "No name";
    }

    if (strcmp(currentPreset->getName(), "Emails") == 0)
    {
        return messageCache.getMessage(Messages::Preset_Email_Message);
    }
    else if (strcmp(currentPreset->getName(), "Coding") == 0)
    {
        return messageCache.getMessage(Messages::Preset_Coding_Message);
    }
    else if (strcmp(currentPreset->getName(), "Focus") == 0)
    {
        return messageCache.getMessage(Messages::Preset_Focus_Message);
    }

    return "Unknown preset";
}