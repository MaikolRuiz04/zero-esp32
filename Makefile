# Convenience Makefile wrapping PlatformIO tasks
# Usage:
#   make build     - compile firmware
#   make upload    - upload to board
#   make test      - run hardware (ESP32) unit tests (flashes board)
#   make test-fast - run native host logic tests (no hardware)
#   make clean     - clean build artifacts
#   make monitor   - open serial monitor
#   make ci        - build + test

PIO?=platformio
ENV=esp32dev

# Windows fallback: if platformio isn't on PATH, use the default per-user virtualenv location
ifeq ($(OS),Windows_NT)
	# Allow user override: set PIO in environment to skip this
	ifeq ($(origin PIO), default)
		PIO := $(USERPROFILE)/.platformio/penv/Scripts/platformio.exe
	endif
endif

build:
	$(PIO) run -e $(ENV)

upload:
	$(PIO) run -e $(ENV) --target upload

monitor:
	$(PIO) device monitor -e $(ENV)

clean:
	$(PIO) run -e $(ENV) --target clean

test:
	$(PIO) test -e $(ENV)

test-fast:
	$(PIO) test -e native_test

help:
	@echo Available targets:
	@echo   make build    - compile firmware
	@echo   make upload   - upload to board
	@echo   make monitor  - open serial monitor
	@echo   make test     - build+flash board & run Unity tests on ESP32
	@echo   make test-fast- run native (host) logic tests only
	@echo   make clean    - clean build artifacts
	@echo   make ci       - build + test
	@echo Override PIO path:  make PIO=/full/path/to/platformio.exe test

ci: build test
	@echo "CI pipeline complete."

.PHONY: build upload monitor clean test test-fast ci help
