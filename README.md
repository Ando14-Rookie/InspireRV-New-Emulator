# InspireRV + CH32V003

This repository contains various projects and utilities for
working with the CH32V003 microcontroller.

## New hardware (InspireRV):

Front View|Back View
:--------:|--------:
![alt text](image\image.png)|![alt text](image\image-1.png)

## Project Structure

* `.github`:
  * `workflows`: GitHub Actions workflows.
  * `Doxyfile`: Doxygen configuration.

* `.vscode`:
  * `settings.json`: VSCode settings:

    ```json
    "C_Cpp.default.compilerPath": "riscv-none-elf-gcc",
    ```

  To set the default compiler to `riscv-none-elf-gcc` for IDE integration.

* `ch32v003_stt`
  * Simple spoken digit recognition.
  * Originally from <https://github.com/brian-smith-github/ch32v003_stt>
  * Read its [README.md](ch32v003_stt/STT-README.md) for more information.

* `ch32v003fun`
  * `driver.h`: Contains the most frequently used functions for the CH32V003.
  * `i2c_events.h`: Contains some frequently used I2C functions written manually.
  * `i2c_tx.c`, `i2c_tx.h`, `oled_min.c`, `oled_min.h`: Contains some frequently used functions for the SSD1306 OLED display. Comes from <https://github.com/eric15342335/inspirelab-game>
  * `ws2812b_simple.h`: Contains one function for controlling the WS2812B LEDs.
  You need to declare the following variables in your code:
  In `funconfig.h`:
  
    ```c
    #define FUNCONF_SYSTICK_USE_HCLK 1
    ```

    In your code (e.g. `main.c`):
  
    ```c
    #define WS2812BSIMPLE_IMPLEMENTATION
    // ...
    #include "ws2812b_simple.h"
    ```

  * Originally from the `extralibs` folder in <https://github.com/cnlohr/ch32v003fun>

* `data`
  * `buttons.h`: Button ADC calibration data.

    Contains two sets of data, one for the first prototype
    `InspireMatrix` and one for the second prototype `InspireComputer` (which uses two ADC channels for buttons). If you are using the `InspireComputer`, declare the following in `funconfig.h`:

    ```c
    #define INTERNAL_INSPIRE_MATRIX
    ```

  * `colors.h`: Contains the color palette for the `InspireMatrix` or `InspireComputer`.
  
    One global variable `led_array[]` act as buffer to store the color data to be displayed.
    Provides functions to manipulate the `led_array[]` buffer.

  * `fonts.h`: Display numbers and characters in the size of 3x5 on WS2812B LEDs.

  * `music.h`: Frequencies, durations and functions for playing music using a buzzer.
  To play sound, use `JOY_sound()`.

* `emulator`
  * Support development of basic embedded system software on Windows/MacOS without requiring
  physical hardware.
  * Aims to achieve function compatibility with the `ch32v003fun` library.
  * `adriel's_2026_work`: Handles key press event for WindowsOS, MacOs and InspireRV. It also contains essential functions needed in `new_emulator_system`

* `i2c-comm`
  * Communication between two boards using I2C protocol. The code contains a master and a slave.

* `misc`
  * `libgcc.a` required by the `ch32v003fun` library on MacOS. See [here](misc/README.md) for more information.

* `movingnum`
  * Animations of numbers moving from right to left, bottom to top. Uses math instead of hard coding.
  Demonstrates the use of `fonts.h`.

* `new_emulator_system`
  * **What is Emulator?**
    * A piece of software (or hardware) that allows one device to act exactly like another.
  * Make emulator in VS code's terminal that re presents the 8x8 LED matrix in InspireRV.
  * All logic/code needed to create the emulator is contained here.

* `paint`
  * Paint on `InspireMatrix`.

* `paint-cursor`
  * Draw images on `InspireMatrix` with direction buttons and matrix buttons
  * Connection:
    * PD2: SW (Analog to Digital PIN)
    * PC6: IN (WS2812B Data PIN)

* `rv-asm`
  * Coding RISC-V Compressed instructions on a board with buttons, and showing the result on the matrix.
  * Originally from <https://github.com/mnurzia/rv>

* `rv-dis` (Working in progress)
  * Disassemble RISC-V compressed instructions and print the result on an external OLED display (e.g. SSD1306).
  * Originally from <https://github.com/michaeljclark/riscv-disassembler>

* `testing`
  * This folder contains a bunch of test programs that are subject to change, and are not guaranteed to work for
  your specific hardware.

> [!NOTE]
> Both `savepaint` and `save-rvasm` implements an filesystem structure.

* `savepaint`
  * Add saving and loading features.
  * Combines `paint`, `movingcar`, `snake-game` and `tic-tac-toe`
  * How to play:
    * Before first icon appearing, hold Y to clear all the saved paints.
    * After first icon appearing, use `UP` and `DOWN` to select apps.
    * Use `LEFT` to use the selected app.
    * Integrated `paint`:
      * Press 64 (or `NUM_LEDS` buttons) to toggle between foreground color and background color.
      * Press X or Y to change the foreground or background color.
      * Press `UP` button to load a saved paint.
      * Press `DOWN` button to save the current paint.
    * Integrated `movingcar`:
      * Currently 64 buttons are divided into left and right section.
      * For each row (8 buttons), left four buttons correspond to `Left Wheel Forward`, `Left Wheel Backward`, `Right Wheel Forward`, `Right Wheel Backward`.
      * For each row (8 buttons), right four buttons correspond to `Red`, `Green`, `Blue` and `Special Command` (Currently not used). The LEDs will display the colors when the motor is running.
      * E.g. if buttons (counting from left) 1, 3, 5 are pressed, the car will go forward and displays red on the entire screen.
      * Special: If all 8 buttons are pressed, the program will go back the first instruction to execute.
    * Integrated `snake-game`:
      * Press Y to start.
      * Use `up / down / left / right` to move the snake.
    * Integrated `tic-tac-toe`:
      * Press any of the empty spot to play.
    * When any of the app ends, press `Y` to go back to the main menu. (`NVIC_SystemReset()` is called)

* `save-rvasm`
  * Add saving and loading features
  * Uses `rv-asm`.
  * How to play:
    * Before the load save menu appear, hold `X` to clear all the saved paints.
    * Pick a saved program to load. Each program size is 32 instructions (hence 64 bytes / 1 page in the filesystem).
    * Press `Y` to load the default program (Smile face) instead of loading a saved program.
      You must choose this if no program are saved.
    * Use `UP` and `DOWN` to navigate the instruction pages. The top 32 LEDs in light pink indicates
      which page is currently displayed. E.g. 1 LED on => page 1 => Instruction 1 and 2.
    * Use `Y` to execute the program.
    * The button `32` LEDs are used to display the current instruction, with the active bit highlighted as `blue`.
    * Notes when writing programs:
      * You `MUST` end your program with an `ecall` (a.k.a `0x0073`) to exit the program.

* `snake-game`
  * Porting the classic snake game to `InspireMatrix`, which has `8x8`=`64` LEDs and `up / down / left / right` controls.

* `tic-tac-toe`
  * Play tic-tac-toe with a bot
  * Press button to start
  * Green is player while Red is the bot
  * Try to win it (very easy)

## What to Setup Beforehand

* [xPack riscv-none-elf-gcc](https://xpack-dev-tools.github.io/riscv-none-elf-gcc-xpack/docs/install/)
  * The RISC-V cross-compiler toolchain. Provides `riscv-none-elf-gcc, riscv-none-elf-size`, etc. The tools your Makefile uses to compile C code into firmware that runs on the CH32V003 chip.
  

* [Zadig](https://zadig.akeo.ie/#)
  * A Windows USB driver switcher. Only needed if you use the `wlink-win-x64 build`. It swaps the WCH-LinkE's driver from WCH's owned driver to WinUSB,so the `wlink CLI` can talk to it. **Not needed** if you use `wlink-win-x86`.
    ![alt text](image.png)
    *  Turn ✔️ the `List All Devices` in Options.
    * Ensure to choose `WCH-Link(Interface 0)` & `WinUSB` as the driver to be switched..

* [wlink](https://github.com/ch32-rs/wlink)
  * An open-source command line tool for flashing firmware to your CH32V003 board via the WCH-LinkE. This is what your `make flash` target calls to automatically `write app.bin` to the chip, no GUI required.

* [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html)
  *  The official GUI flashing tool from WCH. Useful for one-off manual flashing, reading chip info, or updating the WCH-LinkE firmware. Not required if you are using `wlink` for automated `make flash`, but good to have as a backup when something goes wrong.
    ![alt text](image\image-10.png)

## How to compile 

Two options are available for compilation:

* `make`
  * Builds both the firmware and the emulator. After that, it also runs clean, so temporary build files are removed.

* `make firmware`
  * Builds only the firmware for the real RISC-V hardware in real_hardware/. Use this when you want the output files for flashing the physical board.

* `make emu`
  * Builds only the emulator in new_emulator_system/. Use this when you want to test the program on your computer instead of real hardware.

* `make run_emu`
  * Builds the emulator first as prequisite (check if *make emu* has been run or not). Then, it runs *./new_emulator_system/switch_page*.

* `make flash`
  * Builds the firmware first. Then, flash the firmware in `app.bin` to InspireRV.

* `make clean`
  * Removes all compiled files & directory from both the emulator folder and the hardware folder. Use this to clear old build results before compiling again.

Ensure that the **environment** used in terminal is `MSYS2 MinGW64`, otherwise this error below may occur:
* > [auto] Unknown environment: MSYS_NT-10.0-26200

## How to Flash Firmware to InspireRV

* Step 1:
  ![alt text](image/image-2.png)
  * Prepare WCH-LinkE and USB cable extension.
  
* Step 2:

  ![alt text](image\image-3.png)
  * Ensure the working LED mode is Red which means RISCV mode.

* Step 3:

  Side Left View|Side Right View
  :--------:|--------:
  ![alt text](image\image-5.png)|![alt text](image\image-6.png)
  * Connect this way.

* Step 4:   
  * Go back to VS code, and make sure you are in project root path.
  * Setup `Zadig` everytime you want to flash via VS Code. Another way to flash is actually to do it manually via WCHLinkE software.
    ![alt text](image\image-7.png)
  * Type `make flash` or `make auto` using the **MSYS2 MinGW64** compiler (currently used compiler in this project).

## Typical Error
  * **Undetected USB Device**
    * Solution: ensure that the WCH-LinkRV has been updated in `Windows Search>Device Manager Manager>USB devices/USB controller managers`. If it has been updated, the **interface** dropdown list should now include `WCH-LinkRV`
        ![alt text](image\image-9.png)
  * **USB error: incompatible driver is installed for this interface**
    * Solution: reinstall driver with `WCH-LinkRV (Interface 0) --> WinUSB` in the Zadig software.

## Credits

Great thanks to these projects/sources (LICENSE included):

* <https://github.com/cnlohr/ch32v003fun>
* <https://github.com/brian-smith-github/ch32v003_stt>
* <https://github.com/mnurzia/rv>
* <https://github.com/michaeljclark/riscv-disassembler>
* <https://github.com/hexeguitar/ch32v003fun_libs>
* <https://github.com/eric15342335/inspirematrix-buttons/tree/main>
* <https://xpack-dev-tools.github.io/riscv-none-elf-gcc-xpack/docs/install/>
* <https://github.com/ch32-rs/wlink>
* <https://zadig.akeo.ie/#>

## Check out our other projects as well

* <https://github.com/eric15342335/inspirelab-game>
  * A game console based on the `CH32V003J4M6` MCU.
  * Originally from <https://github.com/wagiminator/CH32V003-GameConsole>

* <https://github.com/eric15342335/BitNetMCU>
  * Receives image data via UART and predict the digit using an ML model.
  * Originally from <https://github.com/cpldcpu/BitNetMCU>
