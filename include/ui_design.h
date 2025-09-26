#pragma once
// UI Design Constants

// --- Color Palette (Academia) ---
#define COLOR_FOCUS 0x2105   // Dark green (#142624)
#define COLOR_BREAK 0x4222   // Deep brown (#592C1C)
#define COLOR_SLOT  0x8C69   // Taupe (#8C7968)
#define COLOR_ACCENT_OCHRE 0xA63F // Ochre (#A6783F)
#define COLOR_ACCENT_DUSTY_RED 0x8425 // Dusty red (#8C4B45)

// Aliases for UI elements
#define COLOR_UNDERLINE_FOCUS COLOR_FOCUS // Green underline
#define COLOR_UNDERLINE_BREAK COLOR_BREAK // Brown underline
#define COLOR_TEXT_FOCUS COLOR_FOCUS      // Green text
#define COLOR_TEXT_BREAK COLOR_ACCENT_OCHRE // Ochre text for BREAK

// --- Font & Text Settings ---
#define FONT_SIZE_TASKBAR 2
#define FONT_SIZE_TIMER   5
#define FONT_FAMILY "default" // Adafruit GFX default

// --- Slot/Taskbar Layout ---
#define SLOT_X 6
#define SLOT_Y 8
#define SLOT_W(screenW) ((screenW) - 12)
#define SLOT_H 36
#define SLOT_R 14

// --- Circular Progress ---
// Unified diameter for progress ring (was inconsistently 180 / 190 in code)
#define PROGRESS_DIAMETER 180
// Thickness of the circular progress ring (in pixels)
#define PROGRESS_THICKNESS 8
// Number of discrete segments the progress ring updates through per mode
#define PROGRESS_SEGMENTS 25
// Angle step (degrees) used when drawing arc; smaller -> smoother (performance tradeoff)
#define PROGRESS_ANGLE_STEP 0.35f

// Enable smooth incremental progress updates (draw only the newly added arc each second)
// Set to 0 to fall back to segmented updates (PROGRESS_SEGMENTS)
#define SMOOTH_PROGRESS 1

// --- Rotary Encoder Tuning ---
// Debounce between accepted rotation steps (ms)
#define ENCODER_CFG_STEP_DEBOUNCE_MS 2
// Minimum gap between distinct presses (ms)
#define ENCODER_CFG_PRESS_DEBOUNCE_MS 180
// Stable LOW duration required to qualify as press (ms) (raise slightly for bounce tolerance)
#define ENCODER_CFG_PRESS_STABLE_MS 40
// Set to 1 to invert perceived rotation direction (if wiring orientation feels reversed)
#define ENCODER_CFG_INVERT_DIRECTION 0
// Require DT stability verification (extra noise filtering)
#define ENCODER_CFG_REQUIRE_STABLE_DT 1
// Milliseconds to wait after rotation edge before confirming (0 disables)
#define ENCODER_CFG_ROTATE_STABILITY_MS 1
// Suppress interpreting a press for this window after a confirmed rotation (ms) (reduced to improve fast rotate+press combos)
#define ENCODER_CFG_SUPPRESS_PRESS_AFTER_ROTATE_MS 140
// Ignore rotation edges while a press candidate (potential click) is being evaluated
#define ENCODER_CFG_SUPPRESS_ROTATE_DURING_PRESS 1

// Enable detailed serial logging from rotary encoder module (0 = off, 1 = on)
#ifndef ENCODER_DEBUG
#define ENCODER_DEBUG 0
#endif

// Minimal diagnostic mode: set to 1 to drastically reduce SPI drawing during runtime
// (only a small heartbeat pixel). Helps determine if heavy redraws trigger white screen.
#ifndef MINIMAL_DIAG
#define MINIMAL_DIAG 0
#endif

// Limit SPI clock for display to reduce peak current / signal integrity stress (Hz)
#ifndef TFT_SPI_HZ
#define TFT_SPI_HZ 10000000UL // further reduced for signal/power margin
#endif

// Display watchdog: if no successful UI draw for this many ms, attempt re-init
#ifndef DISPLAY_WATCHDOG_MS
#define DISPLAY_WATCHDOG_MS 4000
#endif
