#include "ui/taskbar.h"
#include "ui/theme_provider.h"
#include "core/timer_controller.h"


#include <TFT_eSPI.h>
#include "../lib/TFT_eSPI/Fonts/GFXFF/RobotoMono_Regular12pt7b.h"

// Academia pastel earthy tones (RGB565)

// Core palette

#include "ui_design.h"

void drawTaskbar(TFT_eSPI &tft, Mode mode, int verticalPad) {
  int slotX = SLOT_X;
  int slotY = SLOT_Y;
  int slotW = SLOT_W(tft.width());
  int slotH = SLOT_H;
  int slotR = SLOT_R;
  // Use ThemeProvider for slot color
  // Slot color: #2E4057 (0x346F) for focus, original break color for break
  uint16_t slotColor = (mode == FOCUS) ? 0x346F : COLOR_SLOT_BREAK;
  tft.fillRoundRect(slotX, slotY, slotW, slotH, slotR, slotColor);

  // --- Taskbar Text: FOCUS (left), BREAK (right) ---
  int pad = 15; // Increased side padding
  tft.setFreeFont(&RobotoMono_Regular12pt7b); // Use smaller RobotoMono Regular
  int fontY = slotY + verticalPad;
  int fontH = slotH - 2 * verticalPad;
  // FOCUS: left aligned, inside slot
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor((mode == FOCUS) ? COLOR_TIMER_TEXT : TFT_BLACK, slotColor);
  tft.drawString("FOCUS", slotX + pad, fontY);
  // BREAK: right aligned, inside slot
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor((mode == BREAK) ? COLOR_TIMER_TEXT : TFT_BLACK, slotColor);
  tft.drawString("BREAK", slotX + slotW - pad, fontY);
  tft.setTextFont(1); // Reset to default
  tft.setTextSize(1);
  tft.setFreeFont(NULL);

  // No underline
}

// Overload for backward compatibility
void drawTaskbar(TFT_eSPI &tft, Mode mode) {
  drawTaskbar(tft, mode, 4); // Default to 4px vertical padding
}
