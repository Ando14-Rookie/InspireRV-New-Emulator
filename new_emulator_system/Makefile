# ─── Builds the PC-side emulator (switch_page.exe) ──────────────────
CC      = gcc # holds the compiler name
CFLAGS  = -Wall -Wextra -g # define how to compile (warnings + debug info)
 # tells the compiler where to find header files (for system_window.h)
INCLUDES = -I"../emulator/adriel_2026_work" \
			-I"../emulator-screen" \
			-I"../emulator" \
			-I"../data" \
			-I"../emulator/adriel_2026_work/emulator_driver" \
			-I"../painting_space" \
			-I"../coding_space"

# tells make where to find source files (for system_window.c)
VPATH    = ../emulator/adriel_2026_work \
			../emulator-screen \
			../emulator/adriel_2026_work/emulator_driver  \
			../emulator \
			../data \
			../painting_space \
			../coding_space

# ─── Files ──────────────────────────────────────────────────────────
TARGET  = switch_page # name of the final executable
# list of source files to compile
SRCS    = switch_page.c \
		../emulator-screen/led_matrix_screen.c \
		../emulator/adriel_2026_work/system_window.c \
		../emulator/adriel_2026_work/extra_function.c \
		../emulator/adriel_2026_work/emulator_driver/emulator_driver.c \
		../emulator/ws2812b_simple.c \
		../data/buttons.c   \
		../data/colors.c   \
		../painting_space/painting_space.c \
		../painting_space/paint_save_space.c \
		../coding_space/coding_space.c

# list of object files generated from source files
OBJS    = switch_page.o \
		led_matrix_screen.o \
		system_window.o \
		extra_function.o \
		ws2812b_simple.o \
		emulator_driver.o \
		buttons.o \
		colors.o \
		painting_space.o \
		paint_save_space.o \
		coding_space.o

# ─── Default rule: build the executable ─────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS) # link the object files into the final executable
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c # pattern rule: how to compile .c files into .o object files
# $@	The target (the file being built)	
# $<	The first prerequisite (the input file)	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# tells make that a target is a "fake" command rather than an actual file. \
It forces the recipe to run even if a file with that name exists.
.PHONY: all clean rebuild emulator

rebuild: clean all

# The test target depending directly on the compiled program
emulator: rebuild # rebuild emulator first before running
	./$(TARGET) --run-tests

# 2. The Clean Target
clean:
	-rm -f *.o *.obj *.exe $(TARGET) emulator switch_page
# How to run:
# 1. Run `make clean` to clean the build artifacts and emulator build.
# 2. Run `make` to clean the emulator build and then build the emulator