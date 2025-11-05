#include "ui/theme_provider.h"
#include "core/timer_controller.h"
#include "slot_progress.h"
#include <math.h>

// Internal state
static int g_totalMs = 0;
static unsigned long g_startMs = 0;
static float g_lastLengthDrawn = 0.0f; // linear length along perimeter already drawn

// Cached geometry
static int g_slotX, g_slotY, g_slotW, g_slotH, g_slotR;
static float g_perimeter = 0.0f;

void slotProgressReset() {
    g_totalMs = 0;
    g_startMs = 0;
    g_lastLengthDrawn = 0.0f;
}

void slotProgressStart(int totalSeconds) {
    g_totalMs = totalSeconds * 1000;
    g_startMs = millis();
    g_lastLengthDrawn = 0.0f;
}

static void computeGeometry(TFT_eSPI &tft) {
    g_slotX = SLOT_X;
    g_slotY = SLOT_Y;
    g_slotW = SLOT_W(tft.width());
    g_slotH = SLOT_H;
    g_slotR = SLOT_R;
    // Perimeter of rounded rectangle
    // Straight segments: 2*(w - 2r) + 2*(h - 2r); corner arcs total = 2*pi*r
    g_perimeter = 2.0f * (g_slotW - 2.0f * g_slotR) + 2.0f * (g_slotH - 2.0f * g_slotR) + 2.0f * 3.14159f * g_slotR;
}

// Draw a stroke segment along an edge (horizontal)
static void drawHSegment(TFT_eSPI &tft, int x0, int x1, int y, uint16_t color) {
    if (x1 < x0) return;
    tft.drawFastHLine(x0, y, x1 - x0 + 1, color);
    for (int i = 1; i < SLOT_PROGRESS_THICKNESS; ++i) {
        tft.drawFastHLine(x0, y + i, x1 - x0 + 1, color);
    }
}

// Vertical segment
static void drawVSegment(TFT_eSPI &tft, int x, int y0, int y1, uint16_t color) {
    if (y1 < y0) return;
    tft.drawFastVLine(x, y0, y1 - y0 + 1, color);
    for (int i = 1; i < SLOT_PROGRESS_THICKNESS; ++i) {
        tft.drawFastVLine(x + i, y0, y1 - y0 + 1, color);
    }
}

// Arc segment: clockwise quarter arc from startAngle to endAngle (radians), centered at cx,cy
static void drawArcSegment(TFT_eSPI &tft, int cx, int cy, float startRad, float endRad, uint16_t color) {
    float step = 57.2957795f / (float)g_slotR; // ~deg per pixel
    float startDeg = startRad * 57.2957795f;
    float endDeg = endRad * 57.2957795f;
    if (endDeg < startDeg) return;
    for (float d = startDeg; d <= endDeg + 0.001f; d += step) {
        float r = d / 57.2957795f;
        int x = cx + (int)(g_slotR * cos(r));
        int y = cy + (int)(g_slotR * sin(r));
        for (int thick = 0; thick < SLOT_PROGRESS_THICKNESS; ++thick) {
            // Simple outward thickness (shift perpendicular could refine later)
            tft.drawPixel(x, y, color);
            // Slight fill: draw a second pixel inward for solidity
            int xIn = cx + (int)((g_slotR - thick) * cos(r));
            int yIn = cy + (int)((g_slotR - thick) * sin(r));
            tft.drawPixel(xIn, yIn, color);
        }
    }
}

// Advance drawing from lastLength toward targetLength.
static void drawProgressDelta(TFT_eSPI &tft, float targetLength, uint16_t color) {
    if (targetLength <= g_lastLengthDrawn) return;
    // Segment ordering definitions (linear lengths):
    float straightTop = g_slotW - 2.0f * g_slotR;
    float arcQuarter = 0.5f * 3.14159f * g_slotR; // each corner arc length = (pi/2)*r
    float straightRight = g_slotH - 2.0f * g_slotR;
    float straightBottom = straightTop;
    float straightLeft = straightRight;

    float L = g_lastLengthDrawn;
    const int maxPixelsPerCall = 300; // safety bound
    int pixelsDrawn = 0;

    while (L < targetLength && pixelsDrawn < maxPixelsPerCall) {
        float remaining = targetLength - L;
        // Determine which segment we're in and draw small chunks (~1 pixel length mapping)
        float nextIncrement = 1.0f; // attempt ~1 px worth of length per iteration

        // Top edge
        if (L < straightTop) {
            float newL = min(straightTop, L + nextIncrement);
            int x0 = g_slotX + g_slotR + (int)L;
            int x1 = g_slotX + g_slotR + (int)(newL);
            drawHSegment(tft, x0, x1, g_slotY, color);
            L = newL;
            pixelsDrawn += (x1 - x0 + 1);
            continue;
        }
        // Top-right arc
        if (L < straightTop + arcQuarter) {
            float offset = L - straightTop;
            float newL = min(straightTop + arcQuarter, L + nextIncrement);
            float offsetNew = newL - straightTop;
            float startAngle = -3.14159f/2.0f + (offset / g_slotR); // from -90deg to 0deg
            float endAngle = -3.14159f/2.0f + (offsetNew / g_slotR);
            int cx = g_slotX + g_slotW - g_slotR;
            int cy = g_slotY + g_slotR;
            drawArcSegment(tft, cx, cy, startAngle, endAngle, color);
            L = newL;
            pixelsDrawn += 1;
            continue;
        }
        // Right edge
        if (L < straightTop + arcQuarter + straightRight) {
            float base = straightTop + arcQuarter;
            float offset = L - base;
            float newL = min(base + straightRight, L + nextIncrement);
            int y0 = g_slotY + g_slotR + (int)offset;
            int y1 = g_slotY + g_slotR + (int)(newL - base);
            int x = g_slotX + g_slotW - SLOT_PROGRESS_THICKNESS; // align at outer right
            drawVSegment(tft, x, y0, y1, color);
            L = newL;
            pixelsDrawn += (y1 - y0 + 1);
            continue;
        }
        // Bottom-right arc
        float seg4Start = straightTop + arcQuarter + straightRight;
        if (L < seg4Start + arcQuarter) {
            float offset = L - seg4Start;
            float newL = min(seg4Start + arcQuarter, L + nextIncrement);
            float offsetNew = newL - seg4Start;
            float startAngle = 0.0f + (offset / g_slotR); // 0 to +90deg
            float endAngle = 0.0f + (offsetNew / g_slotR);
            int cx = g_slotX + g_slotW - g_slotR;
            int cy = g_slotY + g_slotH - g_slotR;
            drawArcSegment(tft, cx, cy, startAngle, endAngle, color);
            L = newL;
            pixelsDrawn += 1;
            continue;
        }
        // Bottom edge (reverse direction right->left)
        float seg5Start = seg4Start + arcQuarter;
        if (L < seg5Start + straightBottom) {
            float base = seg5Start;
            float offset = L - base;
            float newL = min(base + straightBottom, L + nextIncrement);
            int xRight = g_slotX + g_slotW - g_slotR - (int)offset;
            int xLeft = g_slotX + g_slotW - g_slotR - (int)(newL - base);
            drawHSegment(tft, xLeft, xRight, g_slotY + g_slotH - SLOT_PROGRESS_THICKNESS, color);
            L = newL;
            pixelsDrawn += (xRight - xLeft + 1);
            continue;
        }
        // Bottom-left arc
        float seg6Start = seg5Start + straightBottom;
        if (L < seg6Start + arcQuarter) {
            float offset = L - seg6Start;
            float newL = min(seg6Start + arcQuarter, L + nextIncrement);
            float offsetNew = newL - seg6Start;
            // +90deg to +180deg => start at PI/2
            float startAngle = 3.14159f/2.0f + (offset / g_slotR);
            float endAngle = 3.14159f/2.0f + (offsetNew / g_slotR);
            int cx = g_slotX + g_slotR;
            int cy = g_slotY + g_slotH - g_slotR;
            drawArcSegment(tft, cx, cy, startAngle, endAngle, color);
            L = newL;
            pixelsDrawn += 1;
            continue;
        }
        // Left edge (bottom->top)
        float seg7Start = seg6Start + arcQuarter;
        if (L < seg7Start + straightLeft) {
            float base = seg7Start;
            float offset = L - base;
            float newL = min(base + straightLeft, L + nextIncrement);
            int yBottom = g_slotY + g_slotH - g_slotR - (int)offset;
            int yTop = g_slotY + g_slotH - g_slotR - (int)(newL - base);
            int x = g_slotX;
            drawVSegment(tft, x, yTop, yBottom, color);
            L = newL;
            pixelsDrawn += (yBottom - yTop + 1);
            continue;
        }
        // Top-left arc (final closure)
        float seg8Start = seg7Start + straightLeft;
        if (L < seg8Start + arcQuarter) {
            float offset = L - seg8Start;
            float newL = min(seg8Start + arcQuarter, L + nextIncrement);
            float offsetNew = newL - seg8Start;
            // +180deg to +270deg => start at PI
            float startAngle = 3.14159f + (offset / g_slotR);
            float endAngle = 3.14159f + (offsetNew / g_slotR);
            int cx = g_slotX + g_slotR;
            int cy = g_slotY + g_slotR;
            drawArcSegment(tft, cx, cy, startAngle, endAngle, color);
            L = newL;
            pixelsDrawn += 1;
            continue;
        }
        // Safety break if something goes wrong
        break;
    }
    g_lastLengthDrawn = L;
}

void slotProgressUpdate(TFT_eSPI &tft, const TimerController &timer) {
    if (g_totalMs <= 0) return;
    computeGeometry(tft); // cheap calculations, ok each frame
    unsigned long now = millis();
    unsigned long elapsed = (now > g_startMs ? now - g_startMs : 0UL);
    if (elapsed > (unsigned long)g_totalMs) elapsed = g_totalMs;
    float percent = (float)elapsed / (float)g_totalMs;
    if (percent < 0) percent = 0; if (percent > 1) percent = 1;
    float targetLength = percent * g_perimeter;
    uint16_t color = COLOR_TIMER_TEXT;
    drawProgressDelta(tft, targetLength, color);
}
