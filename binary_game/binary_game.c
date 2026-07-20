#include "./binary_game.h"

// Function & variable prototype declaration
// Randomize question number
static int randomNumber; // Returns 0 to 15
static inline void renderBinaryGame(uint8_t selectedNumber);
static inline void checkUserInputCol(uint8_t value, int col[8]);
static inline void renderUserInput(void);
static inline void handleScenario(uint8_t idx);

// Private to this file
typedef enum { BINARY_GAME_IDLE = 0, BINARY_GAME_CORRECT, BINARY_GAME_WRONG } GameState;

void initBinaryGame(void) {

    // Check if user has answered correctly or just wanna quit the game
    bool continuePlay = false;
    // Only move when pointer moves or user select something
    int buttonPressed = 0;
    int enterPressed = 0;
    // Seed the random number generator using the current time
    srand(time(NULL));
    // Randomize question number
    randomNumber = rand() % 16; // Returns 0 to 15

    printf("Random number generated is %d \n", randomNumber);

    // Make all LED off first in `led_array`
    fill_color(offColor);

    // As soon as user enter this program, print binary game
    renderBinaryGame(randomNumber);

    // The game will keep running until user get the answer correct, unless
    // they wish to stop the game
    while (!continuePlay && currentPage == BINARY_GAME) {
        // Activate keyboard I, J, K, L press input
        checkMoveButton();

        // Navigate pointers
        if (BTN_JUST_PRESSED(BTN_UP)) {
            currentposition = (NUM_LEDS + currentposition + 8) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(BTN_DOWN)) {
            currentposition = (NUM_LEDS + currentposition - 8) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(BTN_LEFT)) {
            currentposition = (NUM_LEDS + currentposition + 1) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(BTN_RIGHT)) {
            currentposition = (NUM_LEDS + currentposition - 1) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(Enter_Key)) {
            currentposition = (NUM_LEDS + currentposition - 1) % NUM_LEDS;
            enterPressed = 1;
        }

        // Compute which row and col your pointer is in now
        uint8_t row = currentposition / GRID_COLS;
        uint8_t col = currentposition % GRID_COLS;

        // Print same binary number, user input and button until user confirm or leave

        // Update to compare button released and pressed state
        updateMoveButton();

        // Show and print the BRIGHTNESS_CONTROL state
        // Only works after user move the pointer one by one
        if (buttonPressed == 1) {       
            // Render normally
            renderBinaryGame(randomNumber);
            // Reset state
            buttonPressed = 0;
        }

        // Check what is being pressed
            if (row == 1 && enterPressed == 1) {
                // Handle each scenario
                handleScenario(col);
                // Continue rendering normally
                renderBinaryGame(randomNumber);
                enterPressed = 0;
            }

        // Handle the quit button

        // Return page to previous screen
        if (currentPage == PAINTING_SPACE) {
            // Draw real painting canvas data with brightness applied
            for (int i = 0; i < NUM_LEDS; i++) {
                setColorLEDScaled(i, savedColor[i], brightnessDivisor);
            }
        }
    }
}

/** 
 * @brief  Show and print the `BINARY_GAME` screen everytime pointer moves or answered
 * @param selectedNumber Which random binary question to be shown
 **/ 
static inline void renderBinaryGame(uint8_t selectedNumber) {
    // Type of color that should be turned on
    // val can be 0, 1, 2, 3
    int val = -1;

    // Erase old screen, make it all black
    fill_color(offColor);
    // Handle the Row 1
    renderUserInput();

    // Draw the hardcoded S logo on rows 1-7
    for (int arrayRow = 0; arrayRow < 5; arrayRow++) {
        // array[0] = bottom = LED row 7, so flip the row
        int ledRow = 7 - arrayRow;
        for (int arrayCol = 0; arrayCol < 8; arrayCol++) {
            // col[0] = rightmost = LED col 7, so flip the col
            int ledCol = 7 - arrayCol;

            // Get the saveConfirmLogo[slotIndex][7][7] or the 64th LED as
            // starting point
            int idx = ledRow * 8 + ledCol;

            // Get the number state of that LED
            val = slotLogo[selectedNumber][arrayRow][arrayCol];

            // Handle which LED needs to be turned ON based on slotLogo and pointer
            if (val == 1)
                // set_color(idx, pointerColor);
                setColorLEDScaled(idx, defaultLogoColor, brightnessDivisor);
        }
    }

    // Draw pointer ON TOP visually (only effect led_array), doesn't touch
    set_color(currentposition, pointerColor);
    printf("Pointer current position is %d\n ", currentposition);

    // Print the emulator screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/**
 * @brief Handle the user input
 * , confirm button and quit button
 **/
static inline void handleScenario(uint8_t idx) {
    // Flip and tailor idx to rowOneHandle
    uint8_t index = 7-idx;

    // Handle User Input by changing to selected
    // Only works for user input
    if (idx >= 2 && idx <= 5) {
        if (rowOneHandle[idx] == 0) {
            // Change color
            setColorLEDScaled(idx, selectedColor, brightnessDivisor);
            // Change state to selected
            rowOneHandle[idx] = 1;
        }
        else {
            // Change color by returning to normalColor
            setColorLEDScaled(idx, normalColor, brightnessDivisor);
            // Change state to unselected
            rowOneHandle[idx] = 0;
        }
    }
}

/**
 * @brief Call this everytime emulator screen is printed. This print the user input
 * , confirm button and quit button
 **/
static inline void renderUserInput(void) {
    printf("Before render UserInput\n");
    for (int8_t col = 7; col >= 0; col--) {
        // flip to match physical LED ordering
        int ledCol = 7 - col;
        uint8_t idx = 1 * VERTICAL_BUTTONS + ledCol;

        // Columns 2-5 is always purple (default state)
        // If it is selected, it becomes yellow
        if (rowOneHandle[col] == 0) {
            // Default state = purple color
            setColorLEDScaled(idx, normalColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == 1) {
            // input button selected state = yellow color
            setColorLEDScaled(idx, selectedColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == -1) {
            // Quit button state = red color
            setColorLEDScaled(idx, returnColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == -2) {
            // Confirm button state = green color
            setColorLEDScaled(idx, confirmColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == 3) {
            // Unused LEDs
            setColorLEDScaled(idx, offColor, brightnessDivisor);
        }
    }
    printf("After render UserInput\n");
}

/**
 * @brief Handle the logic when user clicks a page button in Row 0
 * @param col The selected column from Row 0
 **/
static inline void checkUserInputCol(uint8_t value, int col[8]) {
    // col[0] is rightmost; col[7] is leftmost
    col[7] = 0;
    col[6] = 0;
    col[5] = (value >> 3) & 1; // bit 3
    col[4] = (value >> 2) & 1; // bit 2
    col[3] = (value >> 1) & 1; // bit 1
    col[2] = (value >> 0) & 1; // bit 0
    col[1] = 0;
    col[0] = 0;
}