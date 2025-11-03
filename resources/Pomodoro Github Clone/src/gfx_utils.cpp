#include "gfx_utils.h"
#include "debug.h"

void drawDebugCrosshair(DISPLAY_CLASS &display, int16_t x, int16_t y, int16_t length, uint16_t color)
{
#ifdef DEBUG
    display.drawFastHLine(x - length, y, 2 * length, color);
    display.drawFastVLine(x, y - length, 2 * length, color);
#endif
}

void drawPattern(DISPLAY_CLASS &display, Pattern pattern, int16_t x, int16_t y, int16_t w, int16_t h)
{
    const uint8_t *patternData = patterns[(int)pattern];
    for (int16_t i = 0; i < h; i++)
    {
        for (int16_t j = 0; j < w; j++)
        {
            if (patternData[i % 8] & (0x80 >> (j % 8)))
            {
                display.drawPixel(x + j, y + i, GxEPD_BLACK);
            }
        }
    }
}

void drawPatternInRoundedArea(DISPLAY_CLASS &display, int16_t startX, int16_t startY, int16_t areaWidth, int16_t areaHeight, int16_t radius, Pattern patternNo)
{
    const int16_t patternWidth = 8;
    const int16_t patternHeight = 8;
    const uint8_t *pattern = patterns[(int)patternNo];

    // Pre-calculate squared radius for circle tests.
    const int16_t rSq = radius * radius;

    // Define centers for the four corners.
    const int16_t centerTL_x = startX + radius;
    const int16_t centerTL_y = startY + radius;
    const int16_t centerTR_x = startX + areaWidth - radius - 1;
    const int16_t centerTR_y = startY + radius;
    const int16_t centerBL_x = startX + radius;
    const int16_t centerBL_y = startY + areaHeight - radius - 1;
    const int16_t centerBR_x = startX + areaWidth - radius - 1;
    const int16_t centerBR_y = startY + areaHeight - radius - 1;

    // Loop through every pixel in the defined area.
    for (int16_t y = startY; y < startY + areaHeight; y++)
    {
        for (int16_t x = startX; x < startX + areaWidth; x++)
        {
            bool drawPixelFlag = true;
            // Top-left corner
            if (x < startX + radius && y < startY + radius)
            {
                int16_t dx = centerTL_x - x;
                int16_t dy = centerTL_y - y;
                if ((dx * dx + dy * dy) > rSq)
                    drawPixelFlag = false;
            }
            // Top-right corner
            else if (x >= startX + areaWidth - radius && y < startY + radius)
            {
                int16_t dx = x - centerTR_x;
                int16_t dy = centerTR_y - y;
                if ((dx * dx + dy * dy) > rSq)
                    drawPixelFlag = false;
            }
            // Bottom-left corner
            else if (x < startX + radius && y >= startY + areaHeight - radius)
            {
                int16_t dx = centerBL_x - x;
                int16_t dy = y - centerBL_y;
                if ((dx * dx + dy * dy) > rSq)
                    drawPixelFlag = false;
            }
            // Bottom-right corner
            else if (x >= startX + areaWidth - radius && y >= startY + areaHeight - radius)
            {
                int16_t dx = x - centerBR_x;
                int16_t dy = y - centerBR_y;
                if ((dx * dx + dy * dy) > rSq)
                    drawPixelFlag = false;
            }

            if (drawPixelFlag)
            {
                // Determine pattern coordinates.
                int patternX = (x - startX) % patternWidth;
                int patternY = (y - startY) % patternHeight;
                // Check the bit (assumes MSB first in each byte).
                if (pattern[patternY] & (0x80 >> patternX))
                {
                    display.drawPixel(x, y, GxEPD_BLACK);
                }
            }
        }
    }
}

void drawProgressBar(DISPLAY_CLASS &display, ProgressBarStyle style, int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius, int16_t progress)
{
    int16_t progressWidth;
    int16_t innerRadius = radius > 2 ? radius - 2 : 0;
    switch (style)
    {
    case ProgressBarStyle::Bordered:
        display.fillRoundRect(x, y, width, height, radius, GxEPD_WHITE);
        display.drawRoundRect(x, y, width, height, radius, GxEPD_BLACK);
        progressWidth = (width - 8) * progress / 100;

        // recalculate inner radius because it is smaller than the outer radius

        drawPatternInRoundedArea(display, x + 4, y + 4, progressWidth, height - 8, innerRadius, Pattern::Dots);
        break;
    case ProgressBarStyle::Borderless:
        display.fillRoundRect(x, y, width, height, radius, GxEPD_WHITE);
        progressWidth = width * progress / 100;

        display.fillRoundRect(x, y, progressWidth, height, radius, GxEPD_BLACK);
        break;
    }
}

Bounds getBounds(DISPLAY_CLASS &display, const char *text, const GFXfont *font)
{
    display.setTextSize(1);
    display.setFont(font);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    return {x1, y1, w, h};
}

Bounds drawText(DISPLAY_CLASS &display, const char *text, int16_t x, int16_t y, const GFXfont *font, uint16_t color)
{
    display.setTextSize(1);
    display.setFont(font);
    display.setTextColor(color);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(x, y);
    display.print(text);

    return {x, y, w, h};
}

Bounds drawBottomAlignedText(DISPLAY_CLASS &display, const char *text, int16_t x, int16_t y, const GFXfont *font, uint16_t color)
{
    display.setTextSize(1);
    display.setFont(font);
    display.setTextColor(color);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(x, y + h);
    display.print(text);

    return {static_cast<int16_t>(x), static_cast<int16_t>(y - h), w, h};
}

Bounds drawCenteredText(DISPLAY_CLASS &display, const char *text, int16_t x, int16_t y, const GFXfont *font, uint16_t color)
{
    display.setTextSize(1);
    display.setFont(font);
    display.setTextColor(color);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    // Correct the y coordinate considering y1 offset (usually negative)
    int16_t correctedY = y - h / 2 - y1;

    display.setCursor(x - w / 2, correctedY);
    display.print(text);

    return {static_cast<int16_t>(x - w / 2), correctedY, w, h};
}
