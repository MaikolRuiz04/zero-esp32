

# ESP32 Pomodoro Timer Project

**Version:** 0.13

This project is a modular Pomodoro timer for the ESP32 with an ILI9341 TFT display, built using PlatformIO. The code is organized for maintainability and easy feature expansion.

## Features
- **Modular UI**: Taskbar, timer display, and circular progress bar are separate components in `src/ui/`.
- **Modes**: Automatically switches between FOCUS and BREAK modes when the timer runs out.
- **Visuals**: Black background, smooth circular progress bar, and centered timer below the taskbar.
- **Customizable**: Progress bar diameter and UI layout can be easily adjusted.
 - **Performance**: Progress ring only fully clears when mode resets, reducing redraw overhead.

## Code Structure
- `src/main.cpp`: Main logic, handles timer, mode switching, and UI updates.
- `src/ui/taskbar.cpp/h`: Renders the top taskbar and highlights the current mode with a centered underline.
- `src/ui/timer_display.cpp/h`: Draws the timer numbers, centered below the taskbar.
- `src/ui/circle_progress.cpp/h`: Draws the circular progress bar, updating every 5 seconds.
- `include/`: For shared header files.
- `lib/`: For custom libraries (if needed).
- `test/`: For unit tests.

## How It Works
1. On startup, the display shows the taskbar, timer, and progress bar.
2. The timer counts down every second, updating only the numbers for a smooth effect.
3. The progress bar updates every 5 seconds, showing elapsed time for the current mode.
4. When the timer reaches zero, the mode switches (FOCUS <-> BREAK), and the UI updates automatically.
5. The underline below the taskbar is centered and sized to match the active mode word.

## Customization
- Change timer durations in `main.cpp` (`focusSeconds`, `breakSeconds`).
- Adjust progress bar diameter in `drawCircleProgress` calls.
 - Change `PROGRESS_DIAMETER` or `PROGRESS_THICKNESS` in `ui_design.h` for ring sizing.
- UI layout and colors can be modified in the respective UI files.

## Getting Started
1. Install PlatformIO and required libraries (Adafruit GFX, Adafruit ILI9341).
2. Connect your ESP32 and TFT display as per pin definitions in `main.cpp`.
3. Build and upload the firmware using PlatformIO.

## Contributing
Feel free to expand features, improve UI, or add new modes. The modular structure makes it easy to add new components or change existing ones.

---
Created by MaikolRuiz04