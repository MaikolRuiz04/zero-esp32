#ifndef GFXUTILS_H
#define GFXUTILS_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>
#include <vector>

#include "defs.h"

enum class ProgressBarStyle
{
    Bordered,
    Borderless
};

enum class Pattern
{
    Solid,
    Stripes,
    Dots,
    Checkerboard,
    DiagonalStripes,
    CrossHatch,
    SparseDots,
    VerySparseDots
};

// define 8x8 patterns
const uint8_t pattern_solid[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// refined stripes: wider bands (upper half and lower half)
const uint8_t pattern_stripes[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F};
// refined dots: softer dot effect
const uint8_t pattern_dots[8] = {0x88, 0x44, 0x22, 0x11, 0x11, 0x22, 0x44, 0x88};
// new sparse dots pattern: dots further apart
const uint8_t pattern_sparse_dots[8] = {0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00};
// new very sparse dots pattern: dots very far apart
const uint8_t pattern_very_sparse_dots[8] = {0x88, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00};
// refined checkerboard: standard alternating bits
const uint8_t pattern_checkerboard[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
// new diagonal stripes: repeated diagonal bands
const uint8_t pattern_diagonal_stripes[8] = {0xC0, 0x30, 0x0C, 0x03, 0xC0, 0x30, 0x0C, 0x03};
// new crosshatch: grid-like pattern with full horizontal bars on top, middle, and bottom
const uint8_t pattern_crosshatch[8] = {0xFF, 0x92, 0x92, 0x92, 0xFF, 0x92, 0x92, 0xFF};

// put all patterns in an array
const std::vector<const uint8_t *> patterns = {
    pattern_solid,
    pattern_stripes,
    pattern_dots,
    pattern_checkerboard,
    pattern_diagonal_stripes,
    pattern_crosshatch,
    pattern_sparse_dots,
    pattern_very_sparse_dots};

void drawDebugCrosshair(DISPLAY_CLASS &display, int16_t x, int16_t y, int16_t length = 8, uint16_t color = GxEPD_BLACK);

void drawPattern(DISPLAY_CLASS &display, Pattern pattern, int16_t x, int16_t y, int16_t w, int16_t h);

void drawPatternInRoundedArea(
    DISPLAY_CLASS &display,
    int16_t startX, int16_t startY,
    int16_t areaWidth, int16_t areaHeight,
    int16_t radius,
    Pattern patternNo);

void drawProgressBar(
    DISPLAY_CLASS &display,
    ProgressBarStyle style,
    int16_t x,
    int16_t y,
    int16_t width,
    int16_t height,
    int16_t radius,
    int16_t progress);

struct Bounds
{
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
};

Bounds getBounds(
    DISPLAY_CLASS &display,
    const char *text,
    const GFXfont *font);

Bounds drawText(
    DISPLAY_CLASS &display,
    const char *text,
    int16_t x,
    int16_t y,
    const GFXfont *font,
    uint16_t color);

Bounds drawBottomAlignedText(DISPLAY_CLASS &display, const char *text, int16_t x, int16_t y, const GFXfont *font, uint16_t color);

Bounds drawCenteredText(DISPLAY_CLASS &display, const char *text, int16_t x, int16_t y, const GFXfont *font, uint16_t color);

#endif