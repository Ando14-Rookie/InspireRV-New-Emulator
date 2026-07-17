# ====================================================================
# 1. PLATFORM DETECTION & AUTO-CONFIGURATION
# ====================================================================

# Check if a specific target platform was passed (e.g., make PLATFORM=WINDOWS)
# If not specified, we attempt to auto-detect the host system.
ifndef PLATFORM
    ifeq ($(OS),Windows_NT)
        PLATFORM = WINDOWS
    else
        UNAME_S := $(shell uname -s)
        ifeq ($(UNAME_S),Darwin)
            PLATFORM = MAC
        else
            PLATFORM = WINDOWS # Fallback/Default
        endif
    endif
endif

# Configure variables based on the chosen platform
ifeq ($(PLATFORM),MAC)
    CC       = gcc
    TARGET   = switch_page
    # macOS needs ApplicationServices framework for CGKeyCode/keyboard tracking
    LIBS     = -framework ApplicationServices -framework AudioUnit -framework CoreAudio -framework CoreFoundation
    RM       = rm -f
else
    # Windows Cross-Compilation (MinGW setup matching your VS Code profile)
    CC       = x86_64-w64-mingw32-gcc
    TARGET   = switch_page.exe
    LIBS     = 
    RM       = rm -f
endif

# ===================================================================
# 2. COMPILER FLAGS & DIRECTORIES
# ====================================================================

CFLAGS   = -Wall -Wextra -g -pthread

# Tells the compiler where to find header files
INCLUDES = -I"../emulator/adriel_2026_work" \
           -I"../emulator-screen" \
           -I"../emulator" \
           -I"../data" \
           -I"../emulator/adriel_2026_work/emulator_driver" \
           -I"../painting_space" \
           -I"../coding_space" \
           -I"../binary_game" \

# Tells make where to find source files contextually
VPATH    = ../emulator/adriel_2026_work \
           ../emulator-screen \
           ../emulator/adriel_2026_work/emulator_driver  \
           ../emulator \
           ../data \
           ../painting_space \
           ../coding_space \
           ../binary_game

# ====================================================================
# 3. FILES
# ====================================================================

SRCS    = switch_page.c \
          ../emulator-screen/led_matrix_screen.c \
          ../emulator/adriel_2026_work/system_window_mac.c \
          ../emulator/adriel_2026_work/extra_function.c \
          ../emulator/adriel_2026_work/emulator_driver/emulator_driver.c \
          ../emulator/ws2812b_simple.c \
          ../data/buttons.c   \
          ../data/colors.c   \
          ../data/music.c   \
          ../painting_space/painting_space.c \
          ../painting_space/paint_save_space.c \
          ../coding_space/code_save_space.c \
          ../coding_space/coding_space.c \
          ../emulator-screen/brightness_control.c \
          ../binary_game/binary_game.c \
          ../binary_game/random_number_canvas.c

OBJS    = switch_page.o \
          led_matrix_screen.o \
          system_window_mac.o \
          extra_function.o \
          ws2812b_simple.o \
          emulator_driver.o \
          buttons.o \
          colors.o \
          music.o \
          painting_space.o \
          paint_save_space.o \
          code_save_space.o \
          coding_space.o \
          brightness_control.o \
          binary_game.o \
          random_number_canvas.o

# Determine where to store the object files
OBJDIR = myObjects

# Create "myObjects/file_name" for each file name
OBJ_PATHS = $(addprefix $(OBJDIR)/, $(OBJS))

# ====================================================================
# 4. BUILD RULES
# ====================================================================

all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJ_PATHS)
	$(CC) $(CFLAGS) $(OBJ_PATHS) -o $(TARGET) $(LIBS)

# Pattern rule: how to compile .c files into .o object files
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Creates the obj/ directory safely if it does not exist
$(OBJDIR): 
	mkdir -p $(OBJDIR)

# Tells make that a target is a "fake" command rather than an actual file. \
It forces the recipe to run even if a file with that name exists.
.PHONY: all clean rebuild emulator mac win

rebuild: clean all

emulator: rebuild
	./$(TARGET) --run-tests

# Shortcut targets to manually force a platform build if needed
mac:
	$(MAKE) PLATFORM=MAC

win:
	$(MAKE) PLATFORM=WINDOWS

# Removes the target binary and the entire object file directory
clean:
	$(RM) *.o *.obj *.exe $(OBJDIR) $(TARGET) emulator switch_page

# # ─── Builds the PC-side emulator (switch_page.exe) ──────────────────
# CC      = gcc # holds the compiler name
# CFLAGS  = -Wall -Wextra -g -pthread # define how to compile (warnings + debug info)
#  # tells the compiler where to find header files (for system_window.h)
# INCLUDES = -I"../emulator/adriel_2026_work" \
# 			-I"../emulator-screen" \
# 			-I"../emulator" \
# 			-I"../data" \
# 			-I"../emulator/adriel_2026_work/emulator_driver" \
# 			-I"../painting_space" \
# 			-I"../coding_space"

# # tells make where to find source files (for system_window.c)
# VPATH    = ../emulator/adriel_2026_work \
# 			../emulator-screen \
# 			../emulator/adriel_2026_work/emulator_driver  \
# 			../emulator \
# 			../data \
# 			../painting_space \
# 			../coding_space
#  # tells the compiler where to find header files (for system_window.h)
# INCLUDES = -I"../emulator/adriel_2026_work" \
# 			-I"../emulator-screen" \
# 			-I"../emulator" \
# 			-I"../data" \
# 			-I"../emulator/adriel_2026_work/emulator_driver" \
# 			-I"../painting_space" \
# 			-I"../coding_space"

# # tells make where to find source files (for system_window.c)
# VPATH    = ../emulator/adriel_2026_work \
# 			../emulator-screen \
# 			../emulator/adriel_2026_work/emulator_driver  \
# 			../emulator \
# 			../data \
# 			../painting_space \
# 			../coding_space

# # ─── Files ──────────────────────────────────────────────────────────
# TARGET  = switch_page # name of the final executable
# # list of source files to compile
# SRCS    = switch_page.c \
# 		../emulator-screen/led_matrix_screen.c \
# 		../emulator/adriel_2026_work/system_window.c \
# 		../emulator/adriel_2026_work/extra_function.c \
# 		../emulator/adriel_2026_work/emulator_driver/emulator_driver.c \
# 		../emulator/ws2812b_simple.c \
# 		../data/buttons.c   \
# 		../data/colors.c   \
# 		../data/music.c   \
# 		../painting_space/painting_space.c \
# 		../painting_space/paint_save_space.c \
# 		../coding_space/code_save_space.c \
# 		../coding_space/coding_space.c \
# 		../emulator-screen/brightness_control.c

# # list of object files generated from source files
# OBJS    = switch_page.o \
# 		led_matrix_screen.o \
# 		system_window.o \
# 		extra_function.o \
# 		ws2812b_simple.o \
# 		emulator_driver.o \
# 		buttons.o \
# 		colors.o \
# 		music.o \
# 		painting_space.o \
# 		paint_save_space.o \
# 		code_save_space.o \
# 		coding_space.o \
# 		brightness_control.o

# # ─── Default rule: build the executable ─────────────────────────────
# all: $(TARGET)

# $(TARGET): $(OBJS) # link the object files into the final executable
# 	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

# %.o: %.c # pattern rule: how to compile .c files into .o object files
# # $@	The target (the file being built)	
# # $<	The first prerequisite (the input file)	
# 	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# # tells make that a target is a "fake" command rather than an actual file. \
# It forces the recipe to run even if a file with that name exists.
# .PHONY: all clean rebuild emulator

# rebuild: clean all

# # The test target depending directly on the compiled program
# emulator: rebuild # rebuild emulator first before running
# # tells make that a target is a "fake" command rather than an actual file. \
# It forces the recipe to run even if a file with that name exists.
# .PHONY: all clean rebuild emulator

# rebuild: clean all

# # The test target depending directly on the compiled program
# emulator: rebuild # rebuild emulator first before running
# 	./$(TARGET) --run-tests

# # 2. The Clean Target
# clean:
# 	-rm -f *.o *.obj *.exe $(TARGET) emulator switch_page
# # 2. The Clean Target
# clean:
# 	-rm -f *.o *.obj *.exe $(TARGET) emulator switch_page
# # How to run:
# # 1. Run `make clean` to clean the build artifacts and emulator build.
# # 2. Run `make` to clean the emulator build and then build the emulator