# How to rebuild the program
# 1) make clean \
  2) make all

# ".elf" is the full build output, containing code plus program structure, symbol information, \
and debug information.
# ".bin" and ".hex"  are simpler deployment formats generated from the ELF. \
A ".bin" is just the raw firmware bytes, while a ".hex" is a text representation that includes addresses and checksums


# ========= edit these first =========
#  Output firmware name
TARGET := app
# Puts all generated files in the build folder.
BUILD_DIR := build

# := means “expand now and store the result immediately.”
# ?= means “use this value only if it was not set already.”

#  Prefix of your RISC-V compiler tools
TOOLPREFIX ?= riscv-none-elf-

# Your linker script
# The linker script controls memory layout, sections, flash, RAM, stack placement, \
and entry point.
LDSCRIPT ?= ./Ld/Link.ld

# Project files
# List of project sources to be compiled
C_SOURCES := ./User/main.c \
	./User/ch32v003fun/ch32v003fun.c \
	./User/hardware_binary_game/binary_game.c \
	./User/data/colors.c \
	./User/data/buttons.c \
	./User/data/music.c \

 # List of assembly source files
ASM_SOURCES := ./Startup/startup_ch32v00x.S
# Leth program know where to find the header
INCLUDES := \
-I"../User/data" \
-I"../User/ch32v003fun" \
-I"../User"  \
-I"../real_hardware/User/hardware_binary_game"

# ========= tools =========
# Defines which tools to use.
CC := $(TOOLPREFIX)gcc # The C compiler
AS := $(TOOLPREFIX)gcc # The assembler step
OBJCOPY := $(TOOLPREFIX)objcopy # Converts output formats
SIZE := $(TOOLPREFIX)size # Prints code/data size

# ========= flags =========
COMMON_FLAGS := -g -Os -ffunction-sections -fdata-sections -Wall -Wextra -flto -ffunction-sections -fdata-sections
# the compiler to generate code for a 32-bit RISC-V CPU with:\
i = base integer instructions\
m = multiply/divide \
a = atomic instructions \
c = compressed instructions \
zicsr = CSR instructions \
-flto = link-time optimizatio by deleting unused code \
zifencei = instruction fence support.

ARCH_FLAGS := -march=rv32ec_zicsr_zifencei -mabi=ilp32e

# Flags for compiling C files.
# -std=gnu11 uses GNU C11, -MMD generates dependency files, and 
# -MP adds dummy rules to make header dependency handling safer.
CFLAGS := $(COMMON_FLAGS) $(ARCH_FLAGS) $(INCLUDES) -std=gnu11 -MMD -MP

# add this only when you want debug prints:
# CFLAGS += -DDEBUG_VERBOSE

# Flags for compiling assembly files.
# "-x assembler-with-cpp": tells GCC to preprocess the assembly source first, \
which is useful for #include and #define.
ASFLAGS := $(COMMON_FLAGS) $(ARCH_FLAGS) $(INCLUDES) -x assembler-with-cpp -MMD -MP

# How to combine all compiled files into the final program for the microcontroller
LDFLAGS := -march=rv32ec_zicsr_zifencei -mabi=ilp32e \
    -T$(LDSCRIPT) \
    -nostartfiles \
    -Wl,--gc-sections \
    -Wl,-Map,$(BUILD_DIR)/$(TARGET).map \
    --specs=nano.specs

# Libraries linked into the final firmware: standard C, math, GCC support library
LDLIBS := -lc -lm -lgcc

# ========= derived lists =========
# Turn each .c file into .o, for ex, main.c --> build/other_root_file/main.o
C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
# Turn each assembly ".S" file into .o like above
ASM_OBJECTS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
# Combines both C_OBJECTS and ASM_OBJECTS into a big list
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)
# Replace every .o in OBJECTS to create a list of dependency files ".d"
DEPS := $(OBJECTS:.o=.d)

# ========= default target =========
# Produce the ELF file, the BIN file, the HEX file, and then run the size target
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex size

# ========= How to create "build" (rules) =========
# Creates the build directory before other generated files are placed inside it
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Tells "Makefile" how to turn any .c file into a corresponding .o file inside $(BUILD_DIR)
# "| $(BUILD_DIR)": Makefile ensures the directory exists before compiling
# Looks for ../User/data/main.c so need to add "../" in the front
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Tells "Makefile" how to turn any Assembly ".S" file into a corresponding .o file inside $(BUILD_DIR)
$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# To create build/app.elf with list in $(OBJECTS) as prequisite
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

# To create build/app.bin with "build/app.elf" as prequisite
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# To create build/app.hex with "build/app.elf" as prequisite
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# Create command of "make size", essentially to run the program where it requires the \
$(BUILD_DIR)/$(TARGET).elf
size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

# Removes the entire $(BUILD_DIR), so it will also remove the .elf, .bin, .hex, .o, .d
# Need to delete the "build" directory instead of the files in the root folder
clean:
	-rm -rf $(BUILD_DIR)

.PHONY: all clean size

# Tells "Make" to read extra dependency files if exist, so it knows where to obtain .h files
-include $(DEPS)