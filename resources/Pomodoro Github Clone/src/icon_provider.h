#ifndef ICON_PROVIDER_H
#define ICON_PROVIDER_H

#include "icon.h"
#include "icons.h"
#include "images.h"

class IconProvider
{
private:
    static IconProvider *instance;
    bool lpeModeEnabled = false;

public:
    static IconProvider *getInstance()
    {
        if (!instance)
        {
            instance = new IconProvider();
        }
        return instance;
    }

    void setLpeMode(bool enabled)
    {
        lpeModeEnabled = enabled;
    }

    bool isLpeModeEnabled() const
    {
        return lpeModeEnabled;
    }

    const unsigned char *getTimerRunningBackgroundImage()
    {
        if (lpeModeEnabled)
        {
            return image_bg_lpe_bubble;
        }
        else
        {
            return image_bg_bubble;
        }
    }

    Icon *getPresetIcon(const char *name)
    {
        if (strcmp(name, "Coding") == 0)
        {
            if (lpeModeEnabled)
            {
                return &icon_lpehacker;
            }
            else
            {
                return &icon_coding;
            }
        }
        else if (strcmp(name, "Emails") == 0)
        {
            if (lpeModeEnabled)
            {
                return &icon_lpetantrum;
            }
            else
            {
                return &icon_email;
            }
        }
        else if (strcmp(name, "Focus") == 0)
        {
            if (lpeModeEnabled)
            {
                return &icon_lpethink;
            }
            else
            {
                return &icon_focus;
            }
        }

        return &icon_lpehacker;
    }
};

#endif // ICON_PROVIDER_H