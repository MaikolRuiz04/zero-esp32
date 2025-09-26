#include "features/image_display.h"
#include <Arduino.h>
#include <TJpg_Decoder.h>
#include <FS.h>
#include <SPIFFS.h>

// Callback for TJpg_Decoder to draw pixels
static Adafruit_ILI9341* tft_ptr = nullptr;
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  if (tft_ptr) tft_ptr->drawRGBBitmap(x, y, bitmap, w, h);
  return 1;
}

void showImage(Adafruit_ILI9341 &tft) {
  tft_ptr = &tft;
  // Mount SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    while (1) delay(1);
  }

  // Set TJpg_Decoder callback
  TJpgDec.setJpgScale(4);
  TJpgDec.setCallback(tft_output);

  // Check if pic.jpg exists in SPIFFS
  if (SPIFFS.exists("/pic.jpg")) {
    Serial.println("pic.jpg found! Drawing /pic.jpg...");
    uint16_t jpgW = 0, jpgH = 0;
    if (TJpgDec.getFsJpgSize(&jpgW, &jpgH, "/pic.jpg") == 0) {
      Serial.print("Original JPG size: "); Serial.print(jpgW); Serial.print("x"); Serial.println(jpgH);
      // Apply same scale factor we set earlier
      uint8_t scale = 4; // kept in sync with setJpgScale(4)
      uint16_t drawW = jpgW / scale;
      uint16_t drawH = jpgH / scale;
      if (drawW > tft.width()) drawW = tft.width();
      if (drawH > tft.height()) drawH = tft.height();
      int16_t x = (tft.width() - drawW) / 2;
      int16_t y = (tft.height() - drawH) / 2;
      int res = TJpgDec.drawFsJpg(x, y, "/pic.jpg");
      Serial.print("TJpgDec draw result: "); Serial.println(res);
    } else {
      Serial.println("Failed to read JPG size, drawing at 0,0");
      TJpgDec.drawFsJpg(0, 0, "/pic.jpg");
    }
  } else {
    Serial.println("pic.jpg NOT found!");
  }
  tft_ptr = nullptr;
}
