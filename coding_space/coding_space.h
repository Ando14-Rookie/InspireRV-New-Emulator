#ifndef CODING_SPACE_H
#define CODING_SPACE_H

#include "../data/buttons.h" // Needed to update which LED to turn on
#include "../data/colors.h"
#include "../data/music.h"
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"

#include <stdint.h>
#include <stdio.h>

/*InspireRV Commands Intrepreter
Coding Mode
Function: 5bits  value: 3bits

Drawing 0b00
00 000 xxx | Fill Screen - xxx:RGB ~ Clear(000)
00 001 00x | Hide/Show(x:0/x:1) Cursor
00 010 xxx | Draw(Pen down) with color - xxx:RGB, xxx:000(Pen Up)
00 011 xxx | Not yet
00 100 xxx | Sound Freq
00 101 xxx | Sound dur
00 110 xxx | Set Program simulation speed - xxx:(0~7) ~ default 4
00 111 xxx | Move Cursor to 0:home position, 1~4: four corners, 7: variable position
*/

// Determine
typedef enum {
    OPCODE_PEN = 0b00,    // 0: Deal with pen, audio, cursor(starting point), speed, etc
    OPCODE_MOVE = 0b01,   // 1: Handle moves in 8 directions
    OPCODE_OPTION = 0b10, // 2: Jump to certain line in specific page
    OPCODE_VARLOOP =
        0b11 // 3: skipIfCarry, set blue/green color, set x/y variable, set loop variable
} OpCode;

#define EMU_OPCODE_FILLSCREEN 0b00000
#define EMU_OPCODE_TURT 0b00001
#define EMU_OPCODE_PENRGB 0b00010
#define EMU_OPCODE_SOUNDFREQ 0b00100
#define EMU_OPCODE_SOUNDDUR 0b00101
#define EMU_OPCODE_PROSPEED 0b00110
#define EMU_OPCODE_TURT_POS 0b00111

/*
Moving 0b01
01 000 xxx | move north xxx:(0~7) steps
01 001 xxx | move ne    xxx:(0~7) steps
01 010 xxx | move east  xxx:(0~7) steps
01 011 xxx | move se    xxx:(0~7) steps
01 100 xxx | move south xxx:(0~7) steps
01 101 xxx | move sw    xxx:(0~7) steps
01 110 xxx | move west  xxx:(0~7) steps
01 111 xxx | move nw    xxx:(0~7) steps
*/

// FD0 = north,
#define EMU_OPCODE_FD0 0b01000
// FD90 = north east
#define EMU_OPCODE_FD45 0b01001
// FD90 = east
#define EMU_OPCODE_FD90 0b01010
// FD180 = south east
#define EMU_OPCODE_FD135 0b01011
// FD180 = south
#define EMU_OPCODE_FD180 0b01100
// FD270 = south west
#define EMU_OPCODE_FD225 0b01101
// FD270 = west
#define EMU_OPCODE_FD270 0b01110
// FD270 = north west
#define EMU_OPCODE_FD315 0b01111

/*
Options 0b10
10 000 000 | End of Program, codes after will not be run
10 001 xxx | Jump to line xxx:(0~7) at Page 1
10 010 xxx | Jump to line xxx:(0~7) at Page 2
10 011 xxx | Jump to line xxx:(0~7) at Page 3
10 100 xxx | Jump to line xxx:(0~7) at Page 4
10 101 xxx | Load saved Program (0~7)
10 110 xxx | Load saved Music (0~7)
10 111 xxx | Load saved Drawing (0~7)
*/

#define EMU_OPCODE_JUMPP1 0b10001
#define EMU_OPCODE_JUMPP2 0b10010
#define EMU_OPCODE_JUMPP3 0b10011
#define EMU_OPCODE_JUMPP4 0b10100
#define EMU_OPCODE_LOADCODE 0b10101
#define EMU_OPCODE_LOADMUSIC 0b10110
#define EMU_OPCODE_LOADPAINT 0b10111
#define EMU_OPCODE_END 0b10000

/*
loop variables 0b11
11 000 xxx | skipifCarry(minus), decrease with xxx:(0~7) until < 0
11 001 xxx | set blue color variable level xxx:(0~7) - dark 0, bright 7
11 010 xxx | set greencolor variable level xxx:(0~7) - dark 0, bright 7
11 011 xxx | set y variable coord xxx:(0~7) - xy-coord for 8x8
11 100 xxx | set red color variable level xxx:(0~7) - dark 0, bright 7
11 101 xxx | set x variable coord xxx:(0~7) - xy-coord for 8x8
11 110 xxx | set looping variable xxx:(0~7), only for looping
11 111 xxx | skipifCarry(plus), increase with xxx:(0~7) until > 7
*/

#define EMU_OPCODE_MINUSSKIP 0b11000
#define EMU_OPCODE_LOOPVAR 0b11101
#define EMU_OPCODE_ADDSKIP 0b11111
// Only works if EMU_OPCODE_PENRGB is set to RED
#define EMU_OPCODE_RVAR 0b11100
// Only works if EMU_OPCODE_PENRGB is set to GREEN
#define EMU_OPCODE_GVAR 0b11010
// Only works if EMU_OPCODE_PENRGB is set to BLUE
#define EMU_OPCODE_BVAR 0b11001
#define EMU_OPCODE_XVAR 0b11110
#define EMU_OPCODE_YVAR 0b11011

#define EMU_DIR_FD0 8
#define EMU_DIR_FD45 7
#define EMU_DIR_FD90 -1
#define EMU_DIR_FD135 -9
#define EMU_DIR_FD180 -8
#define EMU_DIR_FD225 -7
#define EMU_DIR_FD270 1
#define EMU_DIR_FD315 9
#define EMU_DIR_STOP 0

#define TOTAL_CODE_LINE 28
#define TOTAL_CODE_PAGE 4
#define GRID_COLS VERTICAL_BUTTONS
#define GRID_ROWS HORIZONTAL_BUTTONS

typedef struct {          // Structure declaration
    int toggleState;      // Member (int variable)
    color_t currentColor; // Member (color_t variable)
} LEDStructure;

// Structure declaration for determining which direction to move the LED pointer
typedef struct {
    int8_t dRow;
    int8_t dCol;
} DirectionStep;

typedef enum { CANVAS_1 = 0, CANVAS_2, CANVAS_3, CANVAS_4 } CodeCanvas;

// Global simulation state — persists across frames
typedef enum { SIM_IDLE, SIM_RUNNING } SimState;

// Get the id from whatever canvas you are in
extern uint8_t currentCanvas;

// List of column where canvas button exist
extern uint8_t canvasButtonPos[4];

// Determine which button is currently active now
extern uint8_t activeButton;

// Store the each 8-bits opcode from the 7 line in each canva (there are 4 canvas)
extern uint8_t opCodeStorage[4][7][8];

// Store the full 5-bit opcode when passes line i
extern uint8_t opCodeLineStorage[TOTAL_CODE_LINE];

// Store the full 3-bit value/argument when passes line i
extern uint8_t varLineStorage[TOTAL_CODE_LINE];

// Store the 2-bit group of instruction at line i, such as 00, 01, 10, and 11
extern uint8_t opGrpLineStorage[TOTAL_CODE_LINE];

// Global variable to define which currentCodeCanvas you are right now
extern LEDStructure wholeCodeCanvas[TOTAL_CODE_PAGE][NUM_LEDS];

// Counter which line is being run now
extern uint8_t lineRun;

// How far LED pointer needs to move from initial (value can be seen in 0d or 0b)
extern int8_t currentDirection;

extern SimState simState;
extern uint8_t simLineRun;
extern uint8_t simStepsLeft;
extern int8_t simDirection;
// Saved real version: How long to wait before decoding the next line of opcode
extern uint16_t simTimeoutLineCode;
// Mutabel local version: How long to wait before decoding the next line of opcode
extern uint16_t simTimeoutLc;
// Saved real version: how long to wait before moving the pointer one
extern uint16_t simTimeoutVarCode;
// Mutable Local version: how long to wait before moving the pointer one
extern uint16_t simTimeoutVarc;

// Countdown of remaining steps for the current move
extern uint8_t varRun;
// Default speed variable: 0b100 for simulation, can be changed by user in coding space
extern uint8_t speedVar;

// uint8_t jump_variable = 0;
// //uint8_t jump_var_flag = 0;

// Handles emulator sound frequency and duration, default is 1000Hz for 100ms
extern uint16_t soundFreq;
extern uint16_t soundDur;

/**
 * @brief Turn ON/OFF each button with defined green/blue color.
 * Green is for 5 LED in beginning row, rest of LED in that row is blue
 *
 * @param led LED position where pointer is on
 **/
extern void updateCodeLED(uint8_t led);

/**
 * @brief Get the column where the canvas is ON (column 4 to 7 only)
 **/
extern int getActiveCanvas(void);

/**
 * @brief Initalize the screen and further-coming each coding page canvas
 **/
extern void initCodingGrid(void);

/**
 * @brief Print the 4 red LED in row 0 and show which canva is being used
 **/
extern void renderRow0(void);

/** 
 * @brief Used to reset each LED in current canvas into default state (empty).
 * The canvas option selection will remain visible in each canva. 
 **/
extern void resetCanvaScreen(void);

/**
 * @brief After updating one LED, redraw the full coding canvas from your stored data,
 * then draw the pointer on top. It can also be used to redraw the full coding canvas
 * anytime.
 **/
extern void renderCodingCanvas(void);

/**
 * @brief Extract the opcode from each line from each canvas. In total, it can add up to
 * 28 lines.
 **/
extern void resultSimulation(void);

/** 
 * @brief Call this ONCE PER FRAME from your main loop, only while simState == SIM_RUNNING 
 **/
extern void tickStepSimulation(void);

/** 
 * @brief Start the step-by-step simulation with a given speed. Call once, when button 6 is pressed, 
 * to start an animated run. This must also stop taking any keyboard input.
 * @param speedVar The speed variable for the simulation
 **/
extern void startStepSimulation(uint8_t speedVar);

/** 
 * @brief Stop the step-by-step simulation. Call once, when button 4 is pressed, 
 * to stop the animated run.
**/
extern void stopStepSimulation();

#endif
