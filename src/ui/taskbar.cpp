
#include "ui/taskbar.h"
#include <Adafruit_GFX.h>

// Academia pastel earthy tones (RGB565)

// Core palette

#include "ui_design.h"

void drawTaskbar(Adafruit_ILI9341 &tft, Mode mode) {
  int slotX = SLOT_X;
  int slotY = SLOT_Y;
  int slotW = SLOT_W(tft.width());
  int slotH = SLOT_H;
  int slotR = SLOT_R;
  // Draw rounded slot
  uint16_t slotColor = (mode == FOCUS) ? COLOR_ACCENT_OCHRE : COLOR_SLOT;
  tft.fillRoundRect(slotX, slotY, slotW, slotH, slotR, slotColor);
  // Text settings
  tft.setTextSize(FONT_SIZE_TASKBAR);
  // FOCUS
  const char* focusText = "FOCUS";
  int focusLen = 5;
  int charWidth = 6;
  int textSize = 2;
  int focusPixelWidth = focusLen * charWidth * textSize;
  int focusX = slotX + 14;
  int focusY = slotY + 10;
  tft.setTextColor((mode == FOCUS) ? 0x0000 : 0xFFFF); // Black in FOCUS, white in BREAK
  tft.setCursor(focusX, focusY);
  tft.print(focusText);
  // BREAK
  const char* breakText = "BREAK";
  int breakLen = 5;
  int breakPixelWidth = breakLen * charWidth * textSize;
  int breakX = slotX + slotW - breakPixelWidth - 14;
  int breakY = slotY + 10;
  tft.setTextColor((mode == FOCUS) ? 0x0000 : 0xFFFF); // Black in FOCUS, white in BREAK
  tft.setCursor(breakX, breakY);
  tft.print(breakText);
  // Underline below active mode
  int underlineY = slotY + slotH - 7;
  int underlineWidth, underlineX;
  uint16_t underlineColor;
  if (mode == FOCUS) {
    underlineWidth = focusPixelWidth;
    underlineX = focusX;
    underlineColor = COLOR_UNDERLINE_FOCUS;
  } else {
    underlineWidth = breakPixelWidth;
    underlineX = breakX;
    underlineColor = COLOR_UNDERLINE_BREAK;
  }
  tft.drawLine(underlineX, underlineY, underlineX + underlineWidth, underlineY, underlineColor);
}
