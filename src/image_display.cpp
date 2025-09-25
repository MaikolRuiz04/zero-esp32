#include "image_display.h"
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
    Serial.println("pic.jpg found!");
    Serial.println("Drawing /pic.jpg...");
    int orig_w = 2100; // image width
    int orig_h = 1500; // image height
    int x = (tft.width() - orig_w) / 10;
    int y = (tft.height() - orig_h) / 10;
    int res = TJpgDec.drawFsJpg(x, y, "/pic.jpg");
    Serial.print("TJpgDec result: "); Serial.println(res);
  } else {
    Serial.println("pic.jpg NOT found!");
  }
  tft_ptr = nullptr;
}
