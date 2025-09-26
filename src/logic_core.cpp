#include "logic_core.h"
#include <stdio.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

float lc_percentElapsed(int remainingSeconds, int totalSeconds) {
  if (totalSeconds <= 0) return 1.0f;
  int elapsed = totalSeconds - (remainingSeconds < 0 ? 0 : remainingSeconds);
  float p = (float)elapsed / (float)totalSeconds;
  if (p < 0.f) p = 0.f; else if (p > 1.f) p = 1.f;
  return p;
}

int lc_segmentIndex(int remainingSeconds, int totalSeconds, int segments) {
  if (segments <= 0) return 0;
  if (totalSeconds <= 0) return segments; // fully complete
  int elapsed = totalSeconds - (remainingSeconds < 0 ? 0 : remainingSeconds);
  if (elapsed < 0) elapsed = 0;
  if (elapsed > totalSeconds) elapsed = totalSeconds;
  int segLength = totalSeconds / segments;
  if (segLength <= 0) return 0;
  int idx = elapsed / segLength;
  if (idx >= segments) idx = segments - 1;
  return idx;
}

char* lc_formatTime(int totalSeconds, char* buf) {
  if (totalSeconds < 0) totalSeconds = 0;
  int m = totalSeconds / 60;
  int s = totalSeconds % 60;
  if (m > 99) m = 99; // display constraint
  snprintf(buf, 6, "%02d:%02d", m, s);
  return buf;
}

int lc_clampInt(int v, int lo, int hi) {
  if (v < lo) return lo; if (v > hi) return hi; return v;
}

int lc_minutes(int totalSeconds) { if (totalSeconds < 0) totalSeconds = 0; return totalSeconds / 60; }
int lc_seconds(int totalSeconds) { if (totalSeconds < 0) totalSeconds = 0; return totalSeconds % 60; }
