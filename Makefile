# "shell" prints and stores basic information about the system's hardware and operating system
UNAME_S := $(shell uname -s)
# "shell" check and print status of wlink, then silence any error statement
BOARD_CONNECTED := $(shell wlink status >/dev/null 2>&1 && echo yes)
# "shell" check if wlink can be found in the PATH variable
WLINK := $(shell where wlink 2>nul)

# Compares two values for equality. True if WLINK is an empty string
ifeq ($(WLINK),)
    $(warning wlink not found! Please add it to PATH)
else
    $(info wlink found: $(WLINK))
endif

# Run make on all emulator types
all: firmware emu clean

# Compile Windows & MacOS emulator system
emu:
# Only works for Windows now
	$(MAKE) -C new_emulator_system all

# Runs the emulator executable after emu has been compiled (existed)
# Prequisite: "emu" has been compiled 
run_emu: emu
	./new_emulator_system/switch_page

# Compile the firmware for flashing program to InspireRV
firmware:
	$(MAKE) -C real_hardware all

# Compile + flash the firmware for flashing program to InspireRV
# Prequisite: "emu" has been compiled
flash: firmware
	wlink flash --address 0x08000000 ./real_hardware/build/app.bin	

# Clean all the built/compiled files from the Windows/MacOS and physical emulator system
clean:
	cd new_emulator_system && $(MAKE) clean
	cd real_hardware && $(MAKE) clean

# Let program choose 
auto:
ifdef BOARD_CONNECTED
	@echo "[auto] wlink found - flashing to CH32V003..."
	$(MAKE) flash
# If MINGW found in UNAME_S --> currently using Windows, then run it
else ifneq ($(findstring MINGW,$(UNAME_S)),)
	@echo "[auto] Windows detected - building emulator..."
	$(MAKE) emu
	$(MAKE) run_emu
# Logic for macOS
else ifeq ($(UNAME_S),Darwin)
	@echo "[auto] macOS detected - building emulator..."
	$(MAKE) emu
	$(MAKE) run_emu
else
	@echo "[auto] Unknown environment: $(UNAME_S)"
endif
# Tells the computer that a specific name in your Makefile is just a shortcut command
.PHONY: firmware emu all run_emu flash clean auto