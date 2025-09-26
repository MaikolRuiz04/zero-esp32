

# ESP32 Pomodoro Timer

**Version:** 0.15

Modular Pomodoro / focus timer for ESP32 + ILI9341 TFT, designed for smooth incremental UI updates, quick iteration via host (native) logic tests, and clean separation between pure logic and hardware‑dependent code.

## Features
| Area | Highlights |
|------|------------|
| UI Architecture | Taskbar, timer, circular progress are isolated modules under `src/ui/` |
| Modes | Auto cycle FOCUS ↔ BREAK when timer hits zero |
| Rendering | Smooth delta ring or segmented fallback; per‑digit timer repaint to minimize draw time |
| Performance | Avoids full ring redraw except on mode reset; minimal overdraw strategy |
| Customization | Ring diameter/thickness + colors centralized in `ui_design.h` |
| Audio | Non‑blocking buzzer state machine: single beep (FOCUS), double beep (BREAK) |
| Logic Core | Pure functions (`logic_core`) unit tested on host—fast feedback without flashing |
| Testing | Two environments: on‑device (`esp32dev`) and host (`native_test`) |
| Debounce | Non‑blocking button debounce (timestamp based) |
| Maintainability | Constants centralization + separation of pure logic and side‑effects |

## Code Structure
| Path | Purpose |
|------|---------|
| `src/main.cpp` | Runtime orchestration: mode state machine, UI refresh cadence, buzzer triggers |
| `src/ui/taskbar.*` | Top bar & active mode underline |
| `src/ui/timer_display.*` | Optimized mm:ss drawing (only changed digits) |
| `src/ui/circle_progress.*` | Smooth or segmented circular progress ring (delta drawing) |
| `src/features/buzzer.*` | Non‑blocking active buzzer patterns (single / double beep) |
| `src/logic_core.*` | Pure logic (percent, segment index, formatting, clamping) for host tests |
| `include/` | Shared public headers (constants, cross‑module interfaces) |
| `lib/` | (Optional) Private libraries if feature sets grow large |
| `test/` | Unity tests (device + host) |

## Runtime Flow
1. Startup: screen cleared, taskbar + baseline ring + initial timer drawn.
2. Every second: timer decremented, digits diff‑redrawn.
3. Progress ring: updated via delta arc (smooth) or whole segment (segmented mode).
4. Mode end: auto switch, ring + digits reset, audio cue emitted.
5. Buzzer service polled every loop to finish patterns exactly without blocking.

## Customization
| What | Where |
|------|-------|
| Focus / Break lengths | `focusSeconds`, `breakSeconds` in `main.cpp` |
| Ring diameter & thickness | `PROGRESS_DIAMETER`, `PROGRESS_THICKNESS` in `ui_design.h` |
| Smooth vs segmented | Define / undefine `SMOOTH_PROGRESS` in `ui_design.h` |
| Colors / Palette | `ui_design.h` (central color list) |
| Buzzer pattern durations | Constants near top of `main.cpp` (can be extracted) |
| Font / digit rendering | `timer_display.*` |
| Progress delta algorithm | `circle_progress.cpp` (delta vs full) |

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
| Color accents | Edit color defines in `ui_design.h` |

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

## Future Ideas
- Pause / resume & mid-session skip
- Visual warning arc for last N seconds
- Configurable long break cycle (e.g., every 4th focus)
- SPIFFS persistence of user preferences
- Buzzer “pre-finish” ticking pattern
- Additional test coverage (buzzer timing, degenerate ring percentages)

## Testing Strategy
| Layer | Method |
|-------|--------|
| Pure logic (`logic_core`) | Host (`native_test`) environment – milliseconds feedback |
| Integrated UI / timing | On-device tests (Unity) or manual observation |
| Buzzer timing accuracy | Extend with simulated time helper (planned) |

When adding new pure functions, put them in `logic_core.*` or another host-safe module so they can be validated without flashing.

---
Created by MaikolRuiz04