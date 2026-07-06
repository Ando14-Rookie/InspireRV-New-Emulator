#include "coding_space.h"

// Declare/define private variable and function

static inline color_t zoneColor(uint8_t col);
static inline int8_t getDirectionDelta(uint8_t opcode);
static inline uint8_t ledIndex(uint8_t row, uint8_t col);
static inline uint8_t ledInverseIndex(uint8_t row, uint8_t col);
static inline uint8_t opGroupExtraction(uint8_t[8]);
static inline uint8_t opCodeExtraction(uint8_t[8]);
static inline uint8_t varExtraction(uint8_t[8]);
static inline void compileOpCode(void);
static inline void movePointerByUnits(uint8_t *, int8_t);
static inline void handleRow0Click(uint8_t col);
static inline void renderRow0(void);

uint8_t activeButton;
uint8_t varRun = 0;
uint8_t speedVar = 4; // Default speed variable: 0b100 for simulation, can be changed by
                      // user in coding space

// Made only for this coding_space.c file to use
// Default position of the pointer is at (0,0) which is LED 7
static inline uint8_t simPointer = 7;
static inline color_t simPenColor = {.r = 255, .g = 0, .b = 0};
static inline color_t clearColor = {.r = 0, .g = 0, .b = 0};

static inline int8_t rVariable = 7, gVariable = 7, bVariable = 7, xVariable = 4,
                     yVariable = 4, loopVariable = 0;

// Hide the cursor by default if varRun is 0, and conversely
static inline uint8_t simPenStatus = 0;
// Pen is being used to draw or not; 0 = pen up, 1 = pen down with RGB
static inline color_t simPenRGB = {.r = 0, .g = 0, .b = 0};

SimState simState = SIM_IDLE;
uint8_t simLineRun = 0;
uint8_t simStepsLeft = 0;
int8_t simDirection = EMU_DIR_STOP;
uint16_t simTimeoutLc = 0;
uint16_t simTimeoutVarc = 0;
uint16_t simTimeoutLineCode;
uint16_t simTimeoutVarCode;

// How far LED needs to move; starting point is when (x,y) =(0,0) which is LED 7
int8_t currentDirection = 7;

// By default, code canva needs to be in 1st page which is 0
// Initialize the 4 pages containing 64 LEDs
LEDStructure wholeCodeCanvas[TOTAL_CODE_PAGE][NUM_LEDS] = {0};

/* By default, code canvas is only available in:
    Column 7 --> Code Canvas 0
    Column 6 --> Code Canvas 1
    Column 5 --> Code Canvas 2
    Column 4 --> Code Canvas 3
*/
// Refer to the buttons in the hardware itself column 4 to 7
uint8_t canvasButtonPos[4] = {7, 6, 5, 4};

// Get the id from whatever canvas you are in
uint8_t currentCanvas = CANVAS_1;

// Store the each 8-bits opcode from the 7 line in each canva (there are 4 canvas)
uint8_t opCodeStorage[4][7][8] = {0};

// Store the full 5-bit opcode when passes line i
uint8_t opCodeLineStorage[TOTAL_CODE_LINE] = {0};

// Store the full 3-bit value/argument when passes line i
uint8_t varLineStorage[TOTAL_CODE_LINE] = {0};

// Store the 2-bit group of instruction at line i, such as 00, 01, 10, and 11
uint8_t opGrpLineStorage[TOTAL_CODE_LINE] = {0};

// Counter which line is being run now;
uint8_t lineRun = 0;

// Main functions
/*
    How bit is usually formed:
    ... bit 4 bit 3 bit 2 bit 1 bit 0 <--
    etc 16     8     4     2     1 <--
*/

void extractOpCode(void) {
    for (int codeLine = 0; codeLine < TOTAL_CODE_LINE; codeLine++) {
        // Determine which page you are in now
        uint8_t tempPage = codeLine / 7;
        // Determine which line it should store
        uint8_t tempLine =
            7 - (codeLine % 7); // reversed: top-to-bottom instead of bottom-to-top
        // Store the full green 5-bit opcode when passes each line
        opCodeLineStorage[codeLine] = opCodeExtraction(opCodeStorage[tempPage][tempLine]);
        // Store the 2-bit group of instruction at line i, such as 00, 01, 10, and 11
        opGrpLineStorage[codeLine] = opGroupExtraction(opCodeStorage[tempPage][tempLine]);
        // Store the full 3-bit value/argument when passes each line
        varLineStorage[codeLine] = varExtraction(opCodeStorage[tempPage][tempLine]);
        // if(opCode_line_storage[_code_line] > 0)
        // printf("2-bitOP: %d | Line: %d, 5-bitcode: %d, 3-bitvar: %d\n",
        //     opGrpLineStorage[codeLine], codeLine, opCodeLineStorage[codeLine],
        //     varLineStorage[codeLine]);
    }
}

void resultSimulation(void) {
    simPointer = 7; // reset to (0,0) fresh every simulation run
    simPenStatus = 0;
    // Compile the emulator LED into opcode, group, and variable for each line
    // This is done only once at the start of the simulation
    compileOpCode();
    // Extract the information needed for further processing
    extractOpCode();

    // Set each LED color to black
    fill_color(offColor);
    Delay_Ms(100);
    clearScreen();

    // Print Emulator Screen
    renderCodingCanvas();
    printf("Starting simulation...\n");

    // Start led_array with empty space (black color)
    for (int i = 0; i < NUM_LEDS; i++) {
        led_array[i] = offColor;
    }

    // This will run line per line, and will not stop until all lines have been run
    for (int lineRun = 0; lineRun < TOTAL_CODE_LINE; lineRun++) {
        if (opGrpLineStorage[lineRun] == OPCODE_MOVE) {
            // Get the 5-bit opcode and 3-bit variable for the current line
            uint8_t opcode = opCodeLineStorage[lineRun];
            uint8_t zUnits = varLineStorage[lineRun];

            if (opcode < EMU_OPCODE_FD0 || opcode > EMU_OPCODE_FD315) {
                printf("Command not found, do nothing\n");
                continue;
            }

            // Which direction to go based on the opcode, and how many steps to move
            int8_t direction = getDirectionDelta(opcode);
            printf("Moving in direction: %d | Steps: %d\n", direction, zUnits);

            // Run that line for the number of steps specified by zUnits
            for (uint8_t step = 0; step < zUnits; step++) {
                led_array[simPointer] = simPenStatus ? simPenColor : clearColor;
                movePointerByUnits(&simPointer, direction);
            }
            led_array[simPointer] = &simPenRGB ? simPenColor : clearColor;
        }
        else if (opGrpLineStorage[lineRun] == OPCODE_PEN) {
            // Check that specific line
            switch (opCodeLineStorage[lineRun]) {
                // Hide/show pen
                case EMU_OPCODE_TURT:
                    if (varLineStorage[lineRun] == 0b001) {
                        simPenStatus = 1; // Show pen cursor
                    }
                    else if (varLineStorage[lineRun] == 0b000) {
                        simPenStatus = 0; // Hide pen cursor
                    }
                    break;
                case EMU_OPCODE_PROSPEED:
                    printf("Speed change not implemented for this command, skipping\n");
                    break;
                    // case EMU_OPCODE_SOUNDDUR:
                    //     sound_dur = 50+var_line_storage[line_run]*100;
                    //     break;
                    // case EMU_OPCODE_SOUNDFREQ:
                    //     switch(var_line_storage[line_run]){
                    //         case 0:
                    //             sound_freq = NOTE_C4;
                    //             break;
                    //         case 1:
                    //             sound_freq = NOTE_D4;
                    //             break;
                    //         case 2:
                    //             sound_freq = NOTE_E4;
                    //             break;
                    //         case 3:
                    //             sound_freq = NOTE_F4;
                    //             break;
                    //         case 4:
                    //             sound_freq = NOTE_G4;
                    //             break;
                    //         case 5:
                    //             sound_freq = NOTE_A4;
                    //             break;
                    //         case 6:
                    //             sound_freq = NOTE_B4;
                    //             break;
                    //         case 7:
                    //             sound_freq = NOTE_C5;
                    //             break;
                    //         default:
                    //             sound_freq = NOTE_C4;
                    //             break;
                    //     }
                    //     JOY_sound(sound_freq,sound_dur);
                    //     break;

                // Draw pen down with RGB color or pen up
                case EMU_OPCODE_PENRGB:
                    // Make pen status hidden
                    if (varLineStorage[lineRun] == 0) {
                        simPenStatus = 0; // Hide pen cursor
                    }
                    else {
                        simPenStatus = 1; // Show pen cursor
                        // Do AND operation to take top bit
                        if ((varLineStorage[lineRun] & 0x04) == 0x04)
                            simPenColor.r = 36 * rVariable;
                        else
                            simPenColor.r = 0;
                        // Do AND operation to take middle bit
                        if ((varLineStorage[lineRun] & 0x02) == 0x02)
                            simPenColor.g = 36 * gVariable;
                        else
                            simPenColor.g = 0;
                        // Do AND operation to take last bit
                        if ((varLineStorage[lineRun] & 0x01) == 0x01)
                            simPenColor.b = 36 * bVariable;
                        else
                            simPenColor.b = 0;
                        // printf("Leave Color R: %d, G: %d, B: %d\n",simPenColor.r,
                        // simPenColor.g, simPenColor.b);
                    }
                    break;
                    // case _RVCODE_OPCODE_TURT_POS:
                    //     if(turtStatus == 1){
                    //         if(penStatus == 1){
                    //             rv_coding_board[pointerLocation] = (rvCodeParts){'0',
                    //             rvPendownColor};
                    //         }
                    //         else{
                    //             rv_coding_board[pointerLocation] = (rvCodeParts){'0',
                    //             rvClearColor};
                    //         }

                    //     }
                    //     switch(var_line_storage[line_run]){
                    //         case 0:
                    //             pointerLocation = 36;
                    //             break;
                    //         case 1:
                    //             pointerLocation = 56;
                    //             break;
                    //         case 2:
                    //             pointerLocation = 0;
                    //             break;
                    //         case 3:
                    //             pointerLocation = 7;
                    //             break;
                    //         case 4:
                    //             pointerLocation = 63;
                    //             break;
                    //         case 7:
                    //             pointerLocation = (yVariable * 8 + (7-xVariable));
                    //             break;
                    //         default:
                    //             //pointerLocation = pointerLocation;
                    //             break;
                    //     }
                    //     if(turtStatus == 1){
                    //         rv_coding_board[pointerLocation] = (rvCodeParts){'P',
                    //         rvPointerColor}; logoDisplay();
                    //     }

                    //     break;
                    // case _RVCODE_OPCODE_CLRSCREEN:
                    //     if((var_line_storage[line_run]&0x04)==0x04)
                    //         rvPendownColor.r = 36*rVariable;
                    //     else
                    //         rvPendownColor.r = 0;
                    //     if((var_line_storage[line_run]&0x02)==0x02)
                    //         rvPendownColor.g = 36*gVariable;
                    //     else
                    //         rvPendownColor.g = 0;
                    //     if((var_line_storage[line_run]&0x01)==0x01)
                    //         rvPendownColor.b = 36*bVariable;
                    //     else
                    //         rvPendownColor.b = 0;
                    //     for (ptr = (char *)rv_coding_board; ptr < (char
                    //     *)(rv_coding_board + 64);
                    //          ptr += sizeof(rvCodeParts)) {
                    //         *(rvCodeParts *)ptr = (rvCodeParts){'0', rvPendownColor};
                    //     }
                    //     break;
                    // default:
                    //     //currentDirection = _DIR_STOP;
                    //     break;
            }
            // printf("Pen Line %d Code Done, Next Line | Head code %d\n", line_run,
            // pointerLocation);
        }
    }

    // TODO: mirror your OPCODE_PEN / OPCODE_OPTION / OPCODE_VARLOOP
    // handling here (from your main interpreter) if you want pen
    // color changes and jumps reflected in the preview too.

    // Draw pointer last, so it is always visible
    if (simPenStatus == 1) {
        set_color(simPointer, pointerColor);
    }
    else {
        set_color(simPointer, offColor);
    }

    // Send to display
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    printf("Simulation complete.\n");
}

void tickStepSimulation(void) {
    // This function is similar to resultSimulation() but it runs line by line, and is
    // called in led_matrix_screen.c
    /**
     * --simTimeoutLc : subtract 1 from the counter. This represents "one frame of the
     * main loop has passed. simLineRun++ : move to the next line of opcode simTimeoutLc /
     * simTimeoutLineCode: How long to wait before decoding the next line of opcode
     * simTimeoutVarc / simTimeoutVarCode: How long to wait before moving the pointer one
     * more cell
     **/

    if (simState != SIM_RUNNING)
        return;

    if (simLineRun >= TOTAL_CODE_LINE) {
        set_color(simPointer, pointerColor);
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        simState = SIM_IDLE;
        printf("Step simulation complete.\n");
        return;
    }

    // Only decode a new line when we're not mid-move
    if (simStepsLeft == 0) {
        --simTimeoutLc;
        if (simTimeoutLc > 0)
            return; // still waiting for the line delay

        if (opGrpLineStorage[simLineRun] == OPCODE_MOVE) {
            uint8_t opcode = opCodeLineStorage[simLineRun];
            uint8_t zUnits = varLineStorage[simLineRun];

            // Handle invalid opcode: if it's not a move command, just skip to the next
            // line
            if (opcode < EMU_OPCODE_FD0 || opcode > EMU_OPCODE_FD315) {
                simLineRun++;
                simTimeoutLc = simTimeoutLineCode;
                return;
            }

            // Set up the move: how many steps to take, and in which direction
            simDirection = getDirectionDelta(opcode);
            simStepsLeft = zUnits;

            if (zUnits == 0) {
                simLineRun++;
                simTimeoutLc = simTimeoutLineCode;
            }
        }
        // Handle the program speed change command
        else if (opGrpLineStorage[simLineRun] == OPCODE_PEN) {
            // Check that specific line
            switch (opCodeLineStorage[simLineRun]) {
                // Hide/show pen
                case EMU_OPCODE_TURT:
                    if (varLineStorage[simLineRun] == 0b001) {
                        simPenStatus = 1; // Show pen cursor
                    }
                    else if (varLineStorage[simLineRun] == 0b000) {
                        simPenStatus = 0; // Hide pen cursor
                    }
                    simLineRun++;
                    break;
                // Handle the program speed change command
                case EMU_OPCODE_PROSPEED:
                    uint8_t newSpeedVar = varLineStorage[simLineRun];
                    simTimeoutLineCode = 300 - ((int8_t)newSpeedVar - 4) * 40;
                    simTimeoutVarCode = 150 - ((int8_t)newSpeedVar - 4) * 40;
                    // Proceed to nextline
                    simLineRun++;
                    // Reset the countdown timers to the new speed values
                    simTimeoutLc = simTimeoutLineCode;
                    break;
                case EMU_OPCODE_PENRGB:
                    // Make pen status hidden
                    if (varLineStorage[simLineRun] == 0) {
                        simPenStatus = 0; // Hide pen cursor
                    }
                    else {
                        simPenStatus = 1; // Show pen cursor
                        // Do AND operation to take top bit
                        if ((varLineStorage[simLineRun] & 0x04) == 0x04)
                            simPenColor.r = 36 * rVariable;
                        else
                            simPenColor.r = 0;
                        // Do AND operation to take middle bit
                        if ((varLineStorage[simLineRun] & 0x02) == 0x02)
                            simPenColor.g = 36 * gVariable;
                        else
                            simPenColor.g = 0;
                        // Do AND operation to take last bit
                        if ((varLineStorage[simLineRun] & 0x01) == 0x01)
                            simPenColor.b = 36 * bVariable;
                        else
                            simPenColor.b = 0;
                        // printf("Leave Color R: %d, G: %d, B: %d\n",simPenColor.r,
                        // simPenColor.g, simPenColor.b);
                    }
                    // Proceed to next line
                    simLineRun++;
                    break;
            }
            return;
        }
        else {
            // TODO: handle PEN/OPTION/VARLOOP groups the same way
            simLineRun++;
            simTimeoutLc = simTimeoutLineCode;
        }
        return;
    }

    // Mid-move: step one cell per simTimeoutVarCode ticks
    --simTimeoutVarc;
    if (simTimeoutVarc > 0)
        return;

    // Store what color it is in that specific LED
    led_array[simPointer] = simPenStatus ? simPenColor : clearColor;
    // Move the pointer by one unit in the specified direction
    movePointerByUnits(&simPointer, simDirection);

    // Count down the remaining steps for the current move, and reset the step timer
    simStepsLeft--;
    // Refills the countdown timer back to its starting value before moving to next
    // opcode
    simTimeoutVarc = simTimeoutVarCode;

    set_color(simPointer, pointerColor);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

    if (simStepsLeft == 0) {
        simLineRun++;
        simTimeoutLc = simTimeoutLineCode;
    }
}

void startStepSimulation(uint8_t speedVar) {
    // Literally define and declare that the simulation will start running \
    The real code is handled in tickStepSimulation() which is called in led_matrix_screen.c
    compileOpCode();
    extractOpCode();

    // Make every variable default
    simPointer = 7;
    simPenStatus = 0;
    simLineRun = 0;
    simStepsLeft = 0;

    // Calculate how many loop iterations to wait before each step happens
    // how many main-loop ticks to wait before moving on to decode the next line of code
    simTimeoutLineCode = 300 - ((int8_t)speedVar - 4) * 40;
    // how many main-loop ticks to wait before moving the pointer one more cell within the
    // current move command
    simTimeoutVarCode = 150 - ((int8_t)speedVar - 4) * 40;

    simTimeoutLc = simTimeoutLineCode;
    simTimeoutVarc = simTimeoutVarCode;

    fill_color(offColor);
    Delay_Ms(100);
    clearScreen();

    for (int i = 0; i < NUM_LEDS; i++)
        led_array[i] = offColor;

    simState = SIM_RUNNING;
    printf("Starting step simulation...\n");
}

void updateCodeLED(uint8_t led) {
    uint8_t row = led / GRID_COLS;
    uint8_t col = led % GRID_COLS;

    // Row 0 is handled separately
    if (row == 0) {
        handleRow0Click(col);
        return;
    }

    // Rows 1-7: toggle the LED on/off
    if (wholeCodeCanvas[currentCanvas][led].toggleState == 0) {
        // Currently OFF → turn ON with zone color
        wholeCodeCanvas[currentCanvas][led].toggleState = 1;
        // Determine which color will be used
        color_t zone = zoneColor(col);
        // Automatically assign color based on LED position to led_array
        set_color(led, zone);

        // Update the real LED data
        wholeCodeCanvas[currentCanvas][led].currentColor = zone;
    }
    else {
        // Currently ON → turn OFF
        wholeCodeCanvas[currentCanvas][led].toggleState = 0;
        set_color(led, offColor);
        // Update the real LED data
        wholeCodeCanvas[currentCanvas][led].currentColor = offColor;
    }
}

// Start the logic for coding space
void initCodingGrid(void) {
    // Erase old screen, make it all black
    fill_color(offColor);

    // Initialize the 1st time
    renderRow0();
    // Send to hardware
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    // while(clickButton != -1){
    //     return;
    // }
}

int getActiveCanvas(void) {
    activeButton = canvasButtonPos[currentCanvas];
    return activeButton; // Return column of 7,6,5,4
}

void renderCodingCanvas(void) {
    // 1. Draw real coding canvas data first
    for (int i = 0; i < NUM_LEDS; i++) {
        led_array[i] = wholeCodeCanvas[currentCanvas][i].currentColor;
    }

    // 2. Draw row 0 page indicators on top
    renderRow0();

    // 3. Draw pointer last, so it is always visible
    set_color(currentposition, pointerColor);

    // 4. Send to display
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

// Helper functions: only used within this file to simulate/execute a task

/**
 * @brief Move the pointer by a specified number of units in the direction specified by
 * the opcode
 * @param currentPosition Pointer to the current position set; Default is (0,0), which is
 * LED 7
 * @param zUnits The number of units to move the pointer
 * @param opcode The variable opcode specifying the direction to move
 **/
static inline void movePointerByUnits(uint8_t * currentPosition, int8_t direction) {
    int8_t nextPos = *currentPosition + direction;

    if (nextPos < 0 || nextPos >= NUM_LEDS) {
        return; // out of grid entirely, stop
    }

    if (direction == EMU_DIR_FD90 || direction == EMU_DIR_FD270) {
        // East/West must stay on the same row
        if (nextPos / GRID_COLS == (*currentPosition) / GRID_COLS) {
            *currentPosition = nextPos;
        }
    }
    else if (direction == EMU_DIR_FD45 || direction == EMU_DIR_FD135) {
        // NE/SE must not wrap off the left edge
        if ((*currentPosition) % GRID_COLS != 0) {
            *currentPosition = nextPos;
        }
    }
    else if (direction == EMU_DIR_FD225 || direction == EMU_DIR_FD315) {
        // SW/NW must not wrap off the right edge
        if ((*currentPosition) % GRID_COLS != GRID_COLS - 1) {
            *currentPosition = nextPos;
        }
    }
    else {
        *currentPosition = nextPos;
    }
}

/**
 * @brief Calculate the position of LED from 0 to 63
 * @param opcode The opcode specifying the direction to move
 * @return The direction delta for the specified opcode
 **/
static inline int8_t getDirectionDelta(uint8_t opcode) {
    switch (opcode) {
        case EMU_OPCODE_FD0:
            return EMU_DIR_FD0;
        case EMU_OPCODE_FD45:
            return EMU_DIR_FD45;
        case EMU_OPCODE_FD90:
            return EMU_DIR_FD90;
        case EMU_OPCODE_FD135:
            return EMU_DIR_FD135;
        case EMU_OPCODE_FD180:
            return EMU_DIR_FD180;
        case EMU_OPCODE_FD225:
            return EMU_DIR_FD225;
        case EMU_OPCODE_FD270:
            return EMU_DIR_FD270;
        case EMU_OPCODE_FD315:
            return EMU_DIR_FD315;
        default:
            return EMU_DIR_STOP;
    }
}

/**
 * @brief Calculate the position of LED from 0 to 63
 * @param row The row of the LED (0-7)
 * @param col The column of the LED (0-7)
 * @return The index of the LED in the led_array (0-63)
 **/
static inline uint8_t ledIndex(uint8_t row, uint8_t col) {
    return row * VERTICAL_BUTTONS + col;
}

/**
 * @brief Calculate the position of LED from 63 to 0
 * @param row The row of the LED (0-7)
 * @param col The column of the LED (0-7)
 * @return The index of the LED in the led_array (63 to 0)
 **/
static inline uint8_t ledInverseIndex(uint8_t row, uint8_t col) {
    return row * GRID_COLS + (GRID_COLS - 1 - col);
}

/**
 * @brief Determine if a column belongs to Green or Blue zone. It returns a `color_t` type
 **/
static inline color_t zoneColor(uint8_t col) {
    // Column 7, 6, 5, .., 0 in InspireRV
    return (col < 3) ? onColorBlue : onColorGreen;
}

/**
 * @brief Call this everytime emulator screen is printed and whenever currentPage changes
 **/
static inline void renderRow0(void) {
    for (uint8_t col = 0; col < GRID_COLS; col++) {
        uint8_t idx = ledIndex(0, col);

        // Columns 0-3: Empty, locked OFF
        if (col < 4) {
            set_color(idx, offColor);
        }
        else {
            // Columns 4-7: ON, either solid red or light red
            // Page selector buttons
            if (col == getActiveCanvas()) {
                // Active page = solid bright Red
                set_color(idx, solidColorRed);
            }
            else {
                // Inactive pages = light/dim Red
                set_color(idx, lightColorRed);
            }
        }
    }
}

/**
 * @brief Handle the logic when user clicks a page button in Row 0
 *
 * @param col The selected col from Row 1. It can also take current pointer position as
 * argument
 **/
static inline void handleRow0Click(uint8_t col) {
    if (col <= 3) {
        return; // col 0-3 are unclickable, do nothing
    }
    // Do nothing if column of current active canvas is the same as the selected new
    // canvas `col`
    if (getActiveCanvas() == col) {
        printf("Please choose the provided red LED !!");
        return;
    }
    // Switch active coding page
    else {
        if (col == 7) {
            currentCanvas = CANVAS_1;
        }
        else if (col == 6) {
            currentCanvas = CANVAS_2;
        }
        else if (col == 5) {
            currentCanvas = CANVAS_3;
        }
        else if (col == 4) {
            currentCanvas = CANVAS_4;
        }
        // No need to save again, this has been handled somewhere else
    }
}

void compileOpCode(void) {
    // Print all toggle state
    // for(int i = 0; i < NUM_LEDS; i++) {
    //     printf("LED %d toggleState: %d\n", i,
    //     wholeCodeCanvas[currentCanvas][i].toggleState);
    // }
    for (int codeLine = 0; codeLine < TOTAL_CODE_LINE; codeLine++) {
        // Determine which page you are in now
        uint8_t tempPage = codeLine / 6;
        // Determine which line it should store
        uint8_t tempLine =
            7 - (codeLine % 6); // reversed: top-to-bottom instead of bottom-to-top

        // Copy each bit to each line of opCodeStorage from the
        // wholeCodeCanvas[currentCanvas] toggleState
        for (int codeBit = 0; codeBit < 8; codeBit++) {
            // Calculate the index of LED starting from 63 to 0 in the current canvas
            /*opCodeStorage update per bit from 0 to 7, from the
            wholeCodeCanvas[currentCanvas] toggleState where it starts from LED 63, 63,
            .., 56 in the most toppest row, then 55, 54, .., 48 in the second row, and so
            on until the last row 7, 6, .., 0*/
            opCodeStorage[tempPage][tempLine][codeBit] =
                wholeCodeCanvas[tempPage][ledInverseIndex(tempLine, codeBit)].toggleState;
            // printf("opCodeStorage[%d][%d][%d]: %d\n", tempPage, tempLine, codeBit,
            // opCodeStorage[tempPage][tempLine][codeBit]);
        }
    }
}

/**
 * @brief Build the rightmost 2-bits for the leftmost 2-bits of InspireRV opcode It must
 * return `OpCode` enum value
 *
 * @param receivedMessage the 8-bit opcode of each line from the 8x8 LED matrix
 *
 **/
static inline uint8_t opGroupExtraction(uint8_t receivedMessage[8]) {

    // Check what is inside receivedMessage
    // for(int i = 0; i < 8; i++){
    //     printf("receivedMessage[%d]: %d\n", i, receivedMessage[i]);
    // }

    uint8_t extractedCode = 0b00; // 0b00 is same as 0

    // Extract the rightmost 2-bits for the leftmost 2-bits of InspireRV opcode
    if (receivedMessage[0] > 0) { // MSB (bit 1)
        extractedCode |= 2;       // compare 0b00 with 0b10 --> result: 0b10
    }
    if (receivedMessage[1] > 0) { // LSB (bit 0)
        extractedCode |=
            1; // compare 0b10 from previous extractedCode with 0b01 --> result: 0b11
    }

    // This will return in a form of OpCode such as OPCODE_PEN, OPCODE_VARLOOP, etc
    // printf("Extracted 2-bit OpCode: %d\n", extractedCode);
    return extractedCode;
}

/**
 * @brief Build the 5 bits from the starting 8-bit line. It must return a 5-bits value
 * in `binary form`
 *
 * @param receivedMessage the 8-bit opcode of each line from the 8x8 LED matrix
 **/
static inline uint8_t opCodeExtraction(uint8_t receivedMessage[8]) {
    uint8_t extractedCode = 0b00; // 0b00 is same as 0
    // Extract and check the rightmost 5-bits for the leftmost 5-bits of InspireRV opcode
    if (receivedMessage[0] > 0)
        extractedCode |= 16; // bit4 (MSB)
    if (receivedMessage[1] > 0)
        extractedCode |= 8; // bit3
    if (receivedMessage[2] > 0)
        extractedCode |= 4; // bit2
    if (receivedMessage[3] > 0)
        extractedCode |= 2; // bit1
    if (receivedMessage[4] > 0)
        extractedCode |= 1; // bit0 (LSB)

    // After compraring extractedCode with 0b100, 0b010, 0b001, etc --> it will create a
    // final int val turned into OpCode
    // printf("Extracted 5-bit OpCode: %d\n", extractedCode);
    return extractedCode;
}

/**
 * @brief Build a 3-bit variable from receivedMessage[2], [1], and [0]. It must return a
 * 3-bits value in `binary form`
 *
 * @param receivedMessage the 8-bit opcode of each line from the 8x8 LED matrix
 **/
static inline uint8_t varExtraction(uint8_t receivedMessage[8]) {
    uint8_t extractedCode = 0b00; // 0b00 is same as 0
    // Extract and check the leftmost 3-bits for the rightmost 3-bits of InspireRV opcode
    if (receivedMessage[5] > 0)
        extractedCode |= 4; // bit2 (MSB of var)
    if (receivedMessage[6] > 0)
        extractedCode |= 2; // bit1
    if (receivedMessage[7] > 0)
        extractedCode |= 1; // bit0 (LSB)

    // After compraring extractedCode with 0b100, 0b010, 0b001, etc --> it will create a
    // final int val turned into OpCode
    // printf("Extracted 3-bit Var: %d\n", extractedCode);
    return extractedCode;
}