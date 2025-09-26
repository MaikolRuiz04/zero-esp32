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
