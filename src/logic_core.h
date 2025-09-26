#pragma once
#include <stdint.h>

// Pure functions extracted for testability

// Compute percent elapsed given remaining seconds and total seconds.
// Returns value clamped to [0,1].
float lc_percentElapsed(int remainingSeconds, int totalSeconds);

// Compute segment index for segmented progress mode.
// PROGRESS_SEGMENTS equivalent is passed as parameter to avoid header coupling.
int lc_segmentIndex(int remainingSeconds, int totalSeconds, int segments);

// Format mm:ss into provided buffer (must have size >= 6). Returns pointer to buf.
char* lc_formatTime(int totalSeconds, char* buf);

// Clamp utility
int lc_clampInt(int v, int lo, int hi);

// Derive minutes part
int lc_minutes(int totalSeconds);

// Derive seconds remainder part
int lc_seconds(int totalSeconds);
