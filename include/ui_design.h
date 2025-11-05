#define COLOR_SLOT_PROGRESS_FOCUS 0x4208 // Much darker khaki (darker from 0xB531)
#define COLOR_SLOT_PROGRESS_BREAK 0x3907 // Much darker chestnut (darker from 0xA1E5)
#pragma once
// UI Design Constants

// --- Color Palette (Updated) ---
// Source hex codes: EFEEEE, 95D5E6, DBA9BB, B3A589, A53F2B, 2A2D34 converted to RGB565
// Conversion method: 565 = (R>>3)<<11 | (G>>2)<<5 | (B>>3)
// Mappings:
//   Anti-flash white (#EFEEEE) -> 0xEFBD
//   Non Photo blue (#95D5E6)   -> 0x96BC
//   Orchid pink (#DBA9BB)      -> 0xDD57
//   Khaki (#B3A589)            -> 0xB531
//   Chestnut (#A53F2B)         -> 0xA1E5
//   Gunmetal (#2A2D34)         -> 0x2966

#define COLOR_BG 0x0000          // Black background reverted
#define COLOR_FOCUS 0x96BC       // Light blue fill for focus
#define COLOR_BREAK 0xDD57       // Pink fill for break
#define COLOR_SLOT_FOCUS  COLOR_ACCENT_OCHRE // Khaki for focus slot
#define COLOR_SLOT_BREAK  COLOR_ACCENT_DUSTY_RED // Chestnut for break slot
#define COLOR_ACCENT_OCHRE 0xB531 // Khaki accent
#define COLOR_ACCENT_DUSTY_RED 0xA1E5 // Chestnut accent (may be used elsewhere)
#define COLOR_TIMER_TEXT 0xEFBD  // Anti-flash white for timer digits

// Darker variants for progress perimeter strokes (slightly darkened versions)
// Derived by reducing RGB components ~20% before 565 conversion (approximation)
#define COLOR_FOCUS_DARK 0x4A52   // Much darker blue stroke
#define COLOR_BREAK_DARK 0x824B   // Much darker pink stroke


// Aliases for UI elements
#define COLOR_UNDERLINE_FOCUS COLOR_FOCUS // Green underline
#define COLOR_UNDERLINE_BREAK COLOR_BREAK // Brown underline
#define COLOR_TEXT_FOCUS COLOR_FOCUS      // Green text
#define COLOR_TEXT_BREAK COLOR_ACCENT_OCHRE // Ochre text for BREAK

// --- Font & Text Settings ---
#define FONT_SIZE_TASKBAR 2
#define FONT_SIZE_TIMER   10
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
// If set, we draw incremental arc each second (unless continuous is enabled)
#define SMOOTH_PROGRESS 1

// Enable millisecond-based flowing animation (overrides smooth/segmented drawing)
#ifndef CONTINUOUS_PROGRESS
#define CONTINUOUS_PROGRESS 1
#endif

// --- Slot Outline Progress ---
// Thickness (in pixels) of the animated outline that traces the taskbar slot perimeter
#ifndef SLOT_PROGRESS_THICKNESS
#define SLOT_PROGRESS_THICKNESS 4
#endif
// Enable new slot perimeter progress style (set to 1 to use instead of circular progress)
#ifndef USE_SLOT_PROGRESS
#define USE_SLOT_PROGRESS 1
#endif
