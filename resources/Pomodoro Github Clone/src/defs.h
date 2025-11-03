#ifndef DEFS_H
#define DEFS_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>

#include "fonts/FunnelDisplay_Bold24pt7b.h"
#include "fonts/FunnelDisplay_Bold18pt7b.h"
#include "fonts/FunnelDisplay_Regular14pt7b.h"
#include "fonts/HelvetiPixel24pt7b.h"
#include "fonts/HelvetiPixel16pt7b.h"
#include "fonts/FunnelDisplay_Bold48pt7b.h"
#include "fonts/FunnelDisplay_Bold32pt7b.h"
#include "fonts/FunnelDisplay_Bold60pt7b.h"

#define ANNIVERSARY_MODE false

#define TITLE_FONT FunnelDisplay_Bold32pt7b
#define MAIN_FONT FunnelDisplay_Bold24pt7b
#define TEXT_FONT FunnelDisplay_Regular14pt7b
#define SECONDARY_FONT FunnelDisplay_Bold18pt7b
#define SUB_FONT HelvetiPixel24pt7b
#define SMALL_FONT HelvetiPixel16pt7b
#define SEMI_LARGE_FONT FunnelDisplay_Bold48pt7b
#define LARGE_FONT FunnelDisplay_Bold60pt7b

#define REDRAW_INTERVAL_SLOW 1000

#define DISPLAY_CLASS GxEPD2_BW<GxEPD2_426_GDEQ0426T82, 480>

#endif
