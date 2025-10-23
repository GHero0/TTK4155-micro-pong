
# CC := avr-gcc
# CFLAGS := -Os -std=c11 -mmcu=atmega162 -ggdb -Iinclude -ffunction-sections -fdata-sections
# LDFLAGS := -Wl,--gc-sections -Wl,--relax

CC := avr-gcc
CFLAGS := -Os -std=c11 -mmcu=atmega162 -ggdb -Iinclude -ffunction-sections -fdata-sections -fno-split-wide-types -fno-inline-small-functions -fno-tree-loop-distribute-patterns -fno-tree-vectorize -fomit-frame-pointer -fno-builtin
LDFLAGS := -Wl,--gc-sections -Wl,--relax

BUILD_DIR := build
TARGET_DEVICE := m162
PROGRAMMER := atmelice

# Find all .c files in src/, tests/, and subdirectories
SOURCES := $(shell find src tests -name '*.c')
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

.DEFAULT_GOAL := $(BUILD_DIR)/main.hex

# Compile .c to .o, mirroring directory structure under build/
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects and create .hex
$(BUILD_DIR)/main.hex: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BUILD_DIR)/main.elf
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.hex

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(BUILD_DIR)/main.hex
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i

.PHONY: erase
erase:
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -e


# Add size target to check memory usage
.PHONY: size
size: $(BUILD_DIR)/main.elf
	avr-size --mcu=atmega162 $(BUILD_DIR)/main.elf