# ─── Builds the PC-side emulator (switch_page.exe) ──────────────────
CC      = gcc # holds the compiler name
CFLAGS  = -Wall -Wextra -g # define how to compile (warnings + debug info)
INCLUDES = -I"../emulator/adriel's_2026_work" # tells the compiler where to find header files (for system_window.h)
VPATH    = ../emulator/adriel's_2026_work # tells make where to find source files (for system_window.c)

# ─── Files ──────────────────────────────────────────────────────────
TARGET  = switch_page.exe # name of the final executable
OBJS    = switch_page.o system_window.o extra_function.o # list of object files generated from source files

# ─── Default rule: build the executable ─────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS) # link the object files into the final executable
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c # pattern rule: how to compile .c files into .o object files
# $@	The target (the file being built)	
# $<	The first prerequisite (the input file)	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 3. The test target depending directly on the compiled program
emulator: $(TARGET)
	./$(TARGET) --run-tests

clean: # custom clean rule to remove additional files generated during the build process
	del /f /q $(OBJS) $(TARGET) 2>NUL

# How to run:
# 1. Run `make clean` to clean the build artifacts and emulator build.
# 2. Run `make` to clean the emulator build and then build the emulator


# BELOW ARE NOT USED

# Import all compile rules, toolchain paths, all system from the ch32v003 microcontroller. 
# include ../ch32v003fun/ch32v003fun.mk



# Similar to:

# test_2.o: test_2.c ../A/formula.h # rebuild this if either of those files change
# -c ─── Compile flag ─── Compile only — do NOT link yet
# test_2.c ─── Input file ─── The source file being compiled
# -o test_2.o ─── Output name ─── Name the result test_2.o
# 	$(CC) $(CFLAGS) $(INCLUDES) -c test_2.c -o test_2.o

# SRCS    = switch_page.c "../emulator/adriel's_2026_work/system_window.c" # list of source files to compile

# ─── Default rule: build the executable ─────────────────────────────
# emulator: $(TARGET) # the first target is the default when you run 'make' without arguments
# $(TARGET): $(OBJS) # link the object files into the final executable
# # step 1: clean the emulator build
# 	$(MAKE) -f Makefile-emulator clean 
# # same as: gcc -Wall -Wextra -g -I"../A" -c ../A/calculation.c -o calculation.o 
# 	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) 

# # Custom clean rule to remove additional files generated during the build process
# clean:
# 	ifeq (Windows_NT)
# 		del /Q $(TARGET).elf $(TARGET).hex $(TARGET).bin $(TARGET).map $(TARGET).lst \
# 		$(TARGET).o $(TARGET).d $(TARGET).srec $(TARGET).sym $(TARGET).lss
# 	else
# 		rm -f $(TARGET).elf $(TARGET).hex $(TARGET).bin $(TARGET).map $(TARGET).lst \
# 		$(TARGET).o $(TARGET).d $(TARGET).srec $(TARGET).sym $(TARGET).lss
	
# Custom rule name
# emulator:
# # step 1: clean the emulator build
# 	$(MAKE) -f Makefile-emulator clean 
# # step 2: build the emulator
# 	$(MAKE) -f Makefile-emulator