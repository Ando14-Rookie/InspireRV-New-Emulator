#include "./binary_game.h"

// Function & variable prototype declaration
// Randomize question number
static int randomNumber; // Returns 0 to 15
// Check if user has answered correctly or just wanna quit the game
static bool stopPlaying;
static inline void renderBinaryGame(uint8_t selectedNumber);
static inline uint8_t checkUserInputCol(uint8_t receivedUser[]);
static inline void renderUserInput(void);
static inline void handleScenario(uint8_t idx);

// Private to this file
typedef enum { BINARY_GAME_IDLE = 0, BINARY_GAME_CORRECT, BINARY_GAME_WRONG } GameState;

void initBinaryGame(void) {
    // Check if user has answered correctly or just wanna quit the game
    stopPlaying = false;
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
    while (!stopPlaying && currentPage == BINARY_GAME) {
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
                // Undo the flip so it matches rowOneHandle indexing
                uint8_t logicalCol = 7 - col;   
                // Handle each scenario
                handleScenario(logicalCol);
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
    // Reset user input value
    for(int i = 0; i <=7; i++){
        // Reset only the user input value back to 0
        if (rowOneHandle[i] == 1) {
            rowOneHandle[i] = 0;
            // Default state = purple color
            setColorLEDScaled(i, normalColor, brightnessDivisor);
        }
    }
    // As soon as the function stop, render back the real saved canva as seen BELOW

    // Draw pointer ON TOP visually (only effect led_array), doesn't touch
    set_color(currentposition, pointerColor);
    printf("Pointer current position is %d\n ", currentposition);

    // Print the emulator screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
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
 * @brief Handle the user input, confirm button and quit button
 * @param idx Current position in terms of column, range for column is `0 to 7`
 **/
static inline void handleScenario(uint8_t idx) {
    // Handle User Input by changing to selected
    // Only works for user input
    if (idx >= 2 && idx <= 5) {
        if (rowOneHandle[idx] == 0) {
            // Debug
            // printf("Current rowOne is %d \n", rowOneHandle[index]);
            
            // Change state to selected
            rowOneHandle[idx] = 1;
        }
        else {
            // Change state to unselected
            rowOneHandle[idx] = 0;
        }
    }
    // Handle the quit button
    else if(idx == 0){
        printf("QUIT - Stop Playing Binary Game!");
        // Stop playing and go back to the previous page, namely `PAINTING_SPACE`
        currentPage = prevPageState; 
        //Stop the while loop
        stopPlaying = true;
    }
    // Handle the confirm button
    else if(idx == 7){
        printf("CONFIRM - Submitting Current User Answer...");

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
            setColorLEDScaled(idx, binaryGamereturnColor, brightnessDivisor);
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
static inline uint8_t checkUserInputCol(uint8_t receivedUser[]) {
    // col[0] is rightmost; col[7] is leftmost
    uint8_t v = 0;
    for (int i = 0; i <= 3; i++) 
        v |= (receivedUser[i] ? (1 << i) : 0);
    return v;
}