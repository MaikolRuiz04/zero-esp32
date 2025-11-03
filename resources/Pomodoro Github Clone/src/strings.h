#ifndef STRINGS_H
#define STRINGS_H

#include <Arduino.h>
#include <math.h>    // for float_t, double_t
#include <WString.h> // for String
#include <Preferences.h>
#include "preferences_manager.h"
#include <vector>
#include <map>

enum class Messages
{
    TimerWaitingForConfirmationStartOfBreak_Header,
    TimerWaitingForConfirmationEndOfBreak_Header,

    Break_PauseText,
    Break_LongPauseText,

    // Menu items
    MenuItem_Pause,
    MenuItem_Resume,
    MenuItem_BreakNow,
    MenuItem_SkipBreak,
    MenuItem_Cancel,
    MenuItem_BackToPresets,
    MenuItem_RestartTimer,
    MenuItem_StartBreak,

    // Timer states
    TimerState_Paused,

    // Time formats
    TimeFormat_Minutes,
    TimeFormat_Seconds,

    // Preset specific messages
    Preset_Email_Message,
    Preset_Coding_Message,
    Preset_Focus_Message,

    Statistics,
    Statistics_CurrentCycle,
    Statistics_CurrentTime,
    Statistics_CurrentBreakTime,
    Statistics_TotalCycles,
    Statistics_TotalTime,
    Statistics_TotalBreakTime,
};

class MessageCache
{
private:
    std::map<Messages, const char *> cache;

public:
    MessageCache()
    {
    }

    bool isLpeModeEnabled();

    const char *getMessage(Messages message);
    void clearCache(Messages message);
    void clearAllCache();

    std::vector<const char *> getMessages();
};

extern MessageCache messageCache;

#endif