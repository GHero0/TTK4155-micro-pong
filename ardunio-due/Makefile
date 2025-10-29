# Linker script and bootup files
LDSCRIPT = sam/flash.ld
BOOTUP = sam/sam3x/source/exceptions.c sam/sam3x/source/startup_sam3x.c sam/sam3x/source/system_sam3x.c
MCUTYPE = __SAM3X8E__

# List all source files with their relative paths from src/
# Use relative file paths with a backslash before newline
# Do NOT use a backslash after the last file
SOURCE_FILES = \
	$(BOOTUP) \
	src/main.c \
	src/time.c \
	src/drivers/uart.c \
	src/drivers/can_controller.c \
	src/drivers/can_interrupt.c

# Directory setup
VPATH := $(dir $(SOURCE_FILES))
BUILD_DIR := build
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SOURCE_FILES)))

# Tools
CC := arm-none-eabi-gcc
LD := arm-none-eabi-gcc
AR := arm-none-eabi-ar
AS := arm-none-eabi-as

# Output
ELF := $(BUILD_DIR)/main.elf

# Flags
LDFLAGS := -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,--gc-sections
CFLAGS := -mcpu=cortex-m3 -mthumb -g -std=c11 -MMD
CFLAGS += -I sam -I sam/sam3x/include -I sam/sam3x/source -I sam/cmsis
CFLAGS += -I include -I include/drivers
CFLAGS += -D $(MCUTYPE)

.DEFAULT_GOAL := $(ELF)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files and generate dependency info
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# Compile assembly files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) $< -o $@

# Link to create ELF
$(ELF): $(OBJS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	@echo "Build successful: $(ELF)"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: debug
debug: $(ELF)
	if pgrep openocd; then killall -s 9 openocd; fi
	x-terminal-emulator -e openocd -f sam/openocd.cfg -c "program $(ELF) verify"
	sleep 5
	arm-none-eabi-gdb -tui -iex "target extended-remote localhost:3333" $(ELF)
	killall -s 9 openocd

.PHONY: flash
flash: $(ELF)
	if pgrep openocd; then killall -s 9 openocd; fi
	openocd -f sam/openocd.cfg -c "program $(BUILD_DIR)/main.elf verify reset exit"

# Pull in dependencies
-include $(OBJS:.o=.d)