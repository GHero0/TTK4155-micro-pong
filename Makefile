# ===========================================================
# 				MAKEFILE ATMEGA & ARDUINO 
# ===========================================================

.DEFAULT_GOAL := help

.PHONY: help
help:
	@echo "==============================================="
	@echo "Unified Makefile for ATmega162 and Arduino Due"
	@echo "==============================================="
	@echo ""
	@echo "Build targets:"
	@echo "  make atmega        - Build ATmega162 project"
	@echo "  make arduino       - Build Arduino Due project"
	@echo "  make all           - Build both projects"
	@echo ""
	@echo "Flash targets:"
	@echo "  make flash-atmega  - Flash ATmega162"
	@echo "  make flash-arduino - Flash Arduino Due"
	@echo ""
	@echo "Clean targets:"
	@echo "  make clean         - Clean both projects"
	@echo "  make clean-atmega  - Clean only ATmega162"
	@echo "  make clean-arduino - Clean only Arduino Due"
	@echo ""

# ===========================================================
#				ATMEGA162 Configuration
# ===========================================================
ATMEGA_CC := avr-gcc
ATMEGA_CFLAGS := -Os -mmcu=atmega162 -I atmega-162/include -flto -Wall -ffunction-sections -fdata-sections
ATMEGA_LDFLAGS := -flto -Wl,--gc-sections -Wl,--relax

ATMEGA_BUILD_DIR := atmega-162/build
ATMEGA_TARGET_DEVICE := m162
ATMEGA_PROGRAMMER := atmelice

# Find all .c files in atmega-162/src/ and subdirectories
# Tests are in atmega-162/tests/ and not compiled 
ATMEGA_SOURCES := $(shell find atmega-162/src -name '*.c' 2>/dev/null)
ATMEGA_OBJECTS := $(patsubst %.c,$(ATMEGA_BUILD_DIR)/%.o,$(ATMEGA_SOURCES))
ATMEGA_HEX := $(ATMEGA_BUILD_DIR)/main.hex
ATMEGA_ELF := $(ATMEGA_BUILD_DIR)/main.elf

# ===========================================================
#				ARDUINO-DUE Configuration
# ===========================================================

# Linker script
ARDUINO_LDSCRIPT := arduino-due/sam/flash.ld
ARDUINO_MCUTYPE := __SAM3X8E__

# Automatically find ALL .c files in arduino-due/src/ and arduino-due/sam/
ARDUINO_SOURCES := $(shell find arduino-due/src arduino-due/sam/sam3x/source -name '*.c' 2>/dev/null)

# Directory setup
ARDUINO_BUILD_DIR := arduino-due/build
ARDUINO_OBJECTS := $(patsubst %.c,$(ARDUINO_BUILD_DIR)/%.o,$(ARDUINO_SOURCES))

# Tools
ARDUINO_CC := arm-none-eabi-gcc
ARDUINO_LD := arm-none-eabi-gcc

# Output
ARDUINO_ELF := $(ARDUINO_BUILD_DIR)/main.elf

# Flags
ARDUINO_LDFLAGS := -T$(ARDUINO_LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,--gc-sections
ARDUINO_CFLAGS := -mcpu=cortex-m3 -mthumb -g -std=c11 -Wall
ARDUINO_CFLAGS += -I arduino-due/sam -I arduino-due/sam/sam3x/include 
ARDUINO_CFLAGS += -I arduino-due/sam/sam3x/source -I arduino-due/sam/cmsis
ARDUINO_CFLAGS += -I arduino-due/include
ARDUINO_CFLAGS += -D$(ARDUINO_MCUTYPE)

# ===========================================================
# 					ATMEGA162 TARGETS
# ===========================================================

.PHONY: atmega
atmega: $(ATMEGA_HEX)
	@echo "ATmega162 build complete: $(ATMEGA_HEX)"

$(ATMEGA_BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling (ATmega): $<"
	@$(ATMEGA_CC) $(ATMEGA_CFLAGS) -c $< -o $@

$(ATMEGA_HEX): $(ATMEGA_ELF)
	@echo "Creating hex file: $(ATMEGA_HEX)"
	@avr-objcopy -j .text -j .data -O ihex $(ATMEGA_ELF) $(ATMEGA_HEX)

$(ATMEGA_ELF): $(ATMEGA_OBJECTS)
	@echo "Linking (ATmega): $(ATMEGA_ELF)"
	@$(ATMEGA_CC) $(ATMEGA_CFLAGS) $(ATMEGA_LDFLAGS) $(ATMEGA_OBJECTS) -o $(ATMEGA_ELF)

.PHONY: flash-atmega
flash-atmega: $(ATMEGA_HEX)
	@echo "Flashing ATmega162..."
	avrdude -p $(ATMEGA_TARGET_DEVICE) -c $(ATMEGA_PROGRAMMER) -U flash:w:$(ATMEGA_HEX):i

.PHONY: clean-atmega
clean-atmega:
	@echo "Cleaning (ATmega): $(ATMEGA_BUILD_DIR)"
	@rm -rf $(ATMEGA_BUILD_DIR)

.PHONY: size-atmega
size-atmega: $(ATMEGA_ELF)
	@echo "ATmega162 memory usage:"
	@avr-size --mcu=atmega162 $(ATMEGA_ELF)

# ===========================================================
# 					ARDUINO-DUE TARGETS
# ===========================================================

.PHONY: arduino
arduino: $(ARDUINO_ELF)
	@echo "Arduino-Due build complete: $(ARDUINO_ELF)"

$(ARDUINO_BUILD_DIR):
	@mkdir -p $(ARDUINO_BUILD_DIR)

# Pattern rule - mirrors directory structure in build folder
$(ARDUINO_BUILD_DIR)/%.o: %.c | $(ARDUINO_BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling (Arduino): $<"
	@$(ARDUINO_CC) -c $(ARDUINO_CFLAGS) $< -o $@

# Linking
$(ARDUINO_ELF): $(ARDUINO_OBJECTS) | $(ARDUINO_BUILD_DIR)
	@echo "Linking (Arduino): $(ARDUINO_ELF)"
	@$(ARDUINO_LD) $(ARDUINO_LDFLAGS) -o $@ $(ARDUINO_OBJECTS)

.PHONY: flash-arduino
flash-arduino: $(ARDUINO_ELF)
	@echo "Flashing Arduino Due..."
	@if pgrep openocd >/dev/null 2>&1; then killall -9 openocd; fi
	openocd -f arduino-due/sam/openocd.cfg -c "program $(ARDUINO_ELF) verify reset exit"

.PHONY: debug-arduino
debug-arduino: $(ARDUINO_ELF)
	@echo "Starting debug session..."
	@if pgrep openocd >/dev/null 2>&1; then killall -9 openocd; fi
	x-terminal-emulator -e openocd -f arduino-due/sam/openocd.cfg -c "program $(ARDUINO_ELF) verify" &
	sleep 5
	arm-none-eabi-gdb -tui -iex "target extended-remote localhost:3333" $(ARDUINO_ELF)
	killall -9 openocd

.PHONY: clean-arduino
clean-arduino:
	@echo "Cleaning (Arduino): $(ARDUINO_BUILD_DIR)"
	@rm -rf $(ARDUINO_BUILD_DIR)

# ===========================================================
# 					COMBINED TARGETS
# ===========================================================

.PHONY: all
all: atmega arduino
	@echo ""
	@echo "=========================================="
	@echo "Both projects built successfully!"
	@echo "=========================================="

.PHONY: clean
clean: clean-atmega clean-arduino
	@echo "All build artifacts cleaned"

# ===========================================================
# 					DEBUG TARGETS
# ===========================================================

.PHONY: print-arduino
print-arduino:
	@echo "ARDUINO_SOURCES ($(words $(ARDUINO_SOURCES)) files):"
	@for file in $(ARDUINO_SOURCES); do echo "  $$file"; done
	@echo ""
	@echo "ARDUINO_OBJECTS ($(words $(ARDUINO_OBJECTS)) files):"
	@for obj in $(ARDUINO_OBJECTS); do echo "  $$obj"; done

.PHONY: print-atmega
print-atmega:
	@echo "ATMEGA_SOURCES ($(words $(ATMEGA_SOURCES)) files):"
	@for file in $(ATMEGA_SOURCES); do echo "  $$file"; done
	@echo ""
	@echo "ATMEGA_OBJECTS ($(words $(ATMEGA_OBJECTS)) files):"
	@for obj in $(ATMEGA_OBJECTS); do echo "  $$obj"; done