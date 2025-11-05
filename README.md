

# ESP32 Pomodoro Timer

**Version:** 0.20

## v0.20 Major Updates
- Integrated rotary encoder for input/navigation.
- Migrated display library to TFT_eSPI for improved performance and compatibility.
- Changed all UI fonts to RobotoMono (RobotoMono_Regular12pt7b for taskbar, large 7-segment for timer).

ESP32 + ILI9341 Pomodoro timer featuring a perimeter slot progress animation, modular timer controller, persisted settings/statistics, and an updated themeable palette. Built for incremental drawing (low flicker) and rapid host-side iteration.

## Features
| Area | Highlights |
|------|------------|
| UI Architecture | `src/ui/` (taskbar, timer display, slot outline progress, optional circular ring, rotary encoder integration) |
| Progress Style | Perimeter slot trace (rounded rectangle) with anti-flash white progress; legacy circular ring behind flag |
| Modes | Auto cycle FOCUS ↔ BREAK on completion |
| Timer Logic | `TimerController` encapsulates countdown & switch logic |
| Persistence | `Settings` (focus/break minutes) & `Statistics` (cycles, minutes) via Preferences |
| Rendering | Per‑digit timer redraw; incremental (~1 px) perimeter advances each loop; taskbar font is RobotoMono_Regular12pt7b; all fonts now RobotoMono |
| Performance | Bounded pixel work per frame; minimal full-screen redraws |
| Customization | Palette, fonts, thickness macros centralized in `ui_design.h`; slot color for focus mode is #2E4057 (RGB565: 0x346F); slot progress and underline are always anti-flash white; increased slot text padding; migrated to TFT_eSPI |
| Audio | Non‑blocking buzzer (short beep focus, double beep break) |
| Testing | Removed in 0.16 (manual inspection only) |
| Debounce | Timestamp-based button debounce (300 ms) |
| Maintainability | Modules: core, system, ui; reduced globals |

## Code Structure
| Path | Purpose |
|------|---------|
| `src/main.cpp` | Runtime orchestration: mode state machine, UI refresh cadence, buzzer triggers |
| `src/ui/taskbar.*` | Top bar with FOCUS/BREAK label highlighting (active mode label is white, inactive is black); no underline |
| `src/ui/timer_display.*` | Optimized mm:ss drawing (only changed digits) |
| `src/ui/slot_progress.*` | Perimeter slot progress (primary) |
| `src/ui/circle_progress.*` | Legacy circular progress (fallback if `USE_SLOT_PROGRESS`=0) |
| `src/features/buzzer.*` | Non‑blocking buzzer patterns |
| `src/core/timer_controller.*` | Countdown + mode transitions |
| `src/system/settings.*` | Persistent user durations |
| `src/system/statistics.*` | Persistent focus/break minutes & cycles |
| `include/` | Shared public headers (constants, cross‑module interfaces) |
| `lib/` | (Optional) Private libraries if feature sets grow large |
| `test/` | (Removed in 0.16) |

## Runtime Flow
1. Startup: load settings & statistics; draw taskbar and timer; start perimeter progress cycle.
2. Each second: `TimerController.update()` decrements; only changed digits redraw.
3. Each loop: perimeter progress advances toward target length (millisecond based).
4. Taskbar: active mode label (FOCUS or BREAK) is white, inactive is black; slot color for focus mode is #2E4057.
5. Mode completion: statistics update, progress resets, buzzer pattern plays.
6. Buzzer service runs non-blocking to end tones precisely.

## Customization
| What | Where |
|------|-------|
| Focus / Break durations | `Settings` (Preferences) or initial defaults in `main.cpp` |
| Perimeter thickness | `SLOT_PROGRESS_THICKNESS` |
| Circular ring fallback | Set `USE_SLOT_PROGRESS 0` (re-enable ring macros) |
| Smooth ring (legacy) | `SMOOTH_PROGRESS`, `CONTINUOUS_PROGRESS` |
| Colors / Palette | `ui_design.h` (base + dark stroke variants); slot color for focus mode is #2E4057; slot progress and underline are always anti-flash white |
| Buzzer patterns | Constants in `main.cpp` |
| Font sizes | `FONT_SIZE_TIMER`, `FONT_SIZE_TASKBAR` |
| Statistics | Auto-updated on mode switch | 

## Getting Started
1. Install PlatformIO (VS Code extension or CLI).
2. (Optional) Ensure `platformio` + `make` are on PATH.
3. Wire TFT & buzzer per pin constants inside `main.cpp` (adjust if needed).
4. Fast host logic test (no hardware):
	```bash
	make test-fast
	# or
	platformio test -e native_test
	```
5. Full firmware build & upload (no tests):
	```bash
	make build
	make upload
	# or
	platformio run -e esp32dev --target upload
	```
6. On‑device tests (flashes, then runs Unity):
	```bash
	make test
	# or
	platformio test -e esp32dev
	```
7. Serial monitor:
	```bash
	make monitor
	```

## Configuration Highlights
| Toggle | How |
|--------|-----|
| Smooth ring drawing | Set `#define SMOOTH_PROGRESS 1` (or 0 for segmented) |
| Ring geometry | Edit `PROGRESS_DIAMETER`, `PROGRESS_THICKNESS` |
| Session lengths | Edit `focusSeconds`, `breakSeconds` in `main.cpp` |
| Audio timing | Adjust buzzer macros / constants (`BUZZER_BEEP_MS`, etc.) |
| Color accents | Edit color defines in `ui_design.h` (focus slot color: #2E4057/0x346F; progress/underline: anti-flash white) |

## Makefile Targets
| Target | Action |
|--------|--------|
| `make build` | Compile firmware (esp32dev) |
| `make upload` | Build + upload to board |
| `make monitor` | Open serial monitor |
| `make test` | Flash + run Unity tests on device |
| `make test-fast` | Run host logic tests (`native_test`) |
| `make clean` | Remove build artifacts |
| `make ci` | Build + on-device test pipeline |

## Roadmap
- Stats overlay (long press / double press)
- Pause / resume & manual skip
- Long break scheduling (every Nth focus)
- Theme switching (palette sets)
- Encoder-driven mini menu (Timer / Stats / Settings)
- Buzzer pre-finish ticking pattern
- Sprite buffering for heavy outlines
- Extended tests: persistence, buzzer timing, perimeter math edge cases

## Testing Strategy
| Layer | Method |
|-------|--------|
| TimerController / formatting | Host (`native_test`) tests |
| UI incremental drawing | Manual inspection (planned pixel diff harness) |
| Persistence (settings/statistics) | Device tests after simulated changes |
| Buzzer timing | Planned simulated clock injection |

Add new testable pure functions in `core/` or a new `logic/` module. Legacy `logic_core` removed in 0.16.

---
Created by MaikolRuiz04

---
Tip: To revert to circular ring style set `USE_SLOT_PROGRESS` to 0 and rebuild.