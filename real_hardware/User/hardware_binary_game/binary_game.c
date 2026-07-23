#include "./binary_game.h"

// Declare private variable & function to this file
static const uint8_t slotLogoHW[16][5];
// Grabs two different sources of "unpredictable" data and XORs them together
static uint32_t nextRandom;

// Function prototype declaration
static void seedRandomFromHW(void);
static uint16_t getRandom0to15(void);
static void displayBinaryQuestionHW(uint8_t randomQuestion, uint8_t brightnessDivisor);
static void renderUserInput(uint8_t brightnessDivisor);
static bool checkUserInputCol(uint8_t randomQuestion, uint8_t rowUser[8]);
static void flashCorrect(void);
static void flashWrong(void);

// Create canvas for random selection number
static const uint8_t slotLogoHW[16][5] = {
    {0b00111000, 0b00101000, 0b00101000, 0b00101000, 0b00111000}, // 0
    {0b00010000, 0b00110000, 0b00010000, 0b00010000, 0b00111000}, // 1
    {0b00111000, 0b00001000, 0b00111000, 0b00100000, 0b00111000}, // 2
    {0b00111000, 0b00001000, 0b00111000, 0b00001000, 0b00111000}, // 3
    {0b00101000, 0b00101000, 0b00111000, 0b00001000, 0b00001000}, // 4
    {0b00111000, 0b00100000, 0b00111000, 0b00001000, 0b00111000}, // 5
    {0b00111000, 0b00100000, 0b00111000, 0b00101000, 0b00111000}, // 6
    {0b00111000, 0b00001000, 0b00010000, 0b00010000, 0b00010000}, // 7
    {0b00010000, 0b00101000, 0b00111000, 0b00101000, 0b00010000}, // 8
    {0b00111000, 0b00101000, 0b00111000, 0b00001000, 0b00111000}, // 9
    {0b00101110, 0b01101010, 0b00101010, 0b00101010, 0b01111110}, // 10
    {0b00100010, 0b01100110, 0b00100010, 0b00100010, 0b01110111}, // 11
    {0b00101110, 0b01100010, 0b00101110, 0b00101000, 0b01111110}, // 12
    {0b00101110, 0b01100010, 0b00101110, 0b00100010, 0b01111110}, // 13
    {0b00101010, 0b01101010, 0b00101110, 0b00100010, 0b01110010}, // 14
    {0b00101110, 0b01101000, 0b00101110, 0b00100010, 0b01111110}  // 15
};

// `-1` refers to leave button; `-2` refers to confirm button;
// `1` means user input; If turn into `2`, it becomes selected
static int8_t rowOneHandle[8] = {-1, 3, 0, 0, 0, 0, 3, -2};

void renderBinaryGameHW(uint8_t brightnessDivisor) {
    // Store which button is being pressed
    int8_t button = noButtonPressed;
    // Determine to continue the loop or not
    bool stopPlaying = false;
    // Get random seed
    seedRandomFromHW();
    // Randomize number and get the value from 0 to 15
    uint8_t questionIndex = getRandom0to15();

    // #ifdef DEBUG_VERBOSE
    printf("Question value is %d \n", questionIndex);
    // #endif

    // Display binary question first time
    displayBinaryQuestionHW(questionIndex, brightnessDivisor);

    while (!stopPlaying) {

        //  Repeatedly polls/checks the button matrix
        button = matrix_pressed_two();

        // Check if any button is pressed
        // Handle the user input
        if (button != noButtonPressed) {
            // Handle quit
            if (button == 15) {
                stopPlaying=true;
                break;
            }
            else if (button >= 10 && button <= 13) {
                // Ensure that the current button pressed index matches the one in rowOneHandle
                if (rowOneHandle[7 - (button % horizontalButtons)] == 0) {
                    // Debug
                    // printf("Current rowOne is %d \n", rowOneHandle[index]);
                    
                    // Change state to selected
                    rowOneHandle[7 - (button % horizontalButtons)] = 1;

                    // DEBUG
                    // printf("Index %d is changed to 1", (7 - (button % horizontalButtons)));
                }
                else {
                    // Change state to unselected
                    rowOneHandle[7 - (button % horizontalButtons)] = 0;
                                        
                    // DEBUG
                    // printf("Index %d is changed to 0", (7 - (button % horizontalButtons)));
                }
            }
            // Handle confirm
            else if (button == 8) {
                // Check user input with the corresponding `nextRandom` binary question
                switch(checkUserInputCol(questionIndex, rowOneHandle)){
                    case(true):
                        flashCorrect();
                        #ifdef DEBUG_VERBOSE
                            printf("CORRECT - Your answer matches the corresponding decimal value!");
                        #endif
                        stopPlaying = true;
                        break;
                    case(false):
                        flashWrong();
                        #ifdef DEBUG_VERBOSE
                            printf("WRONG - Well played, try again!");
                        #endif
                }
            }
            // Keep showing everytime user select/press something, unless quitting the
            // game
            displayBinaryQuestionHW(questionIndex, brightnessDivisor);
        }
        else {
            // Quit game if u press this
            if (JOY_9_pressed()) {
                // printf("Exit Saving\n");
                break;
            }
        }
        Delay_Ms(200);
    }

    // Reset user input value
    for(int i = 0; i <=7; i++){
        // Reset only the user input value back to 0
        if (rowOneHandle[i] == 1) {
            rowOneHandle[i] = 0;
            // Default state = purple color
            set_color(i, purpleColor, brightnessDivisor);
        }
    }

    // Rerender canvas using real current data
    flushCanvas();
}

static void seedRandomFromHW(void) {
    // Use CH32V003's free-running timer, then XOR with ADC noise as entropy source
    nextRandom = GPIO_analogRead(GPIO_Ain1_A1);
    if (nextRandom == 0)
        nextRandom = 1; // avoid degenerate all-zero state
}

static uint16_t getRandom0to15(void) {
    // Extracts a 4-bit value from the middle-upper bits of the 32-bit state
    nextRandom = nextRandom * 1103515245 + 12345;
    return (nextRandom >> 16) & 0xF; // use upper bits, mask to 0-15
}

static void displayBinaryQuestionHW(uint8_t randomQuestion, uint8_t brightnessDivisor) {
    clear();

    // Handle printing of Row 1
    renderUserInput(brightnessDivisor);

    // Logic for printing the screen
    for (int arrayRow = 0; arrayRow < 5; arrayRow++) {
        int ledRow = 7 - arrayRow; // top (7) down to row 3
        uint8_t rowBits = slotLogoHW[randomQuestion][arrayRow];
        for (int arrayCol = 0; arrayCol < 8; arrayCol++) {
            int ledCol = 7 - arrayCol;
            int idx = ledRow * 8 + ledCol;
            // Bit 7 = column 0, so shift right by (7 - arrayCol) or ledCol
            int val = (rowBits >> (ledCol)) & 1;

            if (val == 1) {
                set_color(idx, blueColor, brightnessDivisor);
            }
        }
    }

    // Send the new led_array to InspireRV
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/**
 * @brief Call this everytime emulator screen is printed. This print the user input
 * , confirm button and quit button
 **/
static void renderUserInput(uint8_t brightnessDivisor) {
    #ifdef DEBUG_VERBOSE
    printf("Before render UserInput\n");
    #endif
    for (int8_t col = 7; col >= 0; col--) {
        // flip to match physical LED ordering
        int ledCol = 7 - col;
        uint8_t idx = verticalButtons + ledCol;

        switch (rowOneHandle[col]) {
            case 0:
                set_color(
                    idx, (color_t){100, 165, 255}, brightnessDivisor); // default = purple
                break;
            case 1:
                set_color(idx, yellowColor,
                    brightnessDivisor); // selected = yellow
                break;
            case -1:
                set_color(idx, redColor,
                    brightnessDivisor); // quit = red (per your comment)
                break;
            case -2:
                set_color(idx, greenColor,
                    brightnessDivisor); // confirm = green (per your comment)
                break;
            case 3:
                set_color(
                    idx, offColor, brightnessDivisor); // unused LEDs = off
                break;
            default:
                break;
        }

        #ifdef DEBUG_VERBOSE
                printf("After render UserInput\n");
        #endif

        // Send the new led_array to InspireRV
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    }
}

/**
 * @brief Handle the logic when user clicks a page button in Row 0
 * @param rowUser The selected column from Row 1
 **/
static inline bool checkUserInputCol(uint8_t randomQuestion, uint8_t rowUser[8]) {
    // col[0] is rightmost; col[7] is leftmost

    /* HOW DOES THE CODE WORK??
    
    It only shifts the literal number 1,  a completely separate, brand-new value, to build the temporary 8-bit binary (0b). 
    The array is only ever read (via rowUser[i]), never shifted.

    | i | rowUser[i] | shift = 5-i | Truthy? | Expression    | v before | v after |
    | - | --------   | ----------- | ------- | ------------- | -------- | ------- |
    | 2 | 0          | 3           | false   | 0             | 0000     | 0000    |
    | 3 | 0          | 2           | false   | 0             | 0000     | 0000    |
    | 4 | 1          | 1           | true    | 1 << 1 = 0010 | 0000     | 0010    |
    | 5 | 1          | 0           | true    | 1 << 0 = 0001 | 0010     | 0011    |
    
    */

    uint8_t v = 0;
    for (int i = 2; i <= 5; i++) {
        int shift = 5 - i;  // index 2 -> shift 3, index 5 -> shift 0
        v |= (rowUser[i] ? (1 << shift) : 0);
    }

    // Compare if user input and binary question is the same or not
    return (v == randomQuestion);
}

/**
 * @brief Create short visual to show if user answer is correct or false
 * @param notes List of notes to use
 * @param duration How long should each note/blinking behaviour last
 * @param len How many times should it loops
 * @param color What color should be used for the blinking
 **/
static inline void playMelodyWithFlash(const uint16_t *notes, const uint8_t *durations, uint8_t len, color_t color) {
    // Clear the screen first
    clear();

    // Run the visual and audio
    for (uint8_t i = 0; i < len; i++) {
        // Fills Screen with Green/Red
        fill_color(color);           

        // Prints the emulator screen
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        
        // Plays note for its own duration (blocking or non-blocking, your driver's call)
        JOY_sound(notes[i], durations[i]);     
        
        // Fills Screen with OFF LED between notes
        fill_color((color_t){0, 0, 0});     

        // Prints the emulator screen again
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        
         // Brief gap so blinks look distinct, not one continuous glow
        Delay_Ms(5);                   
    }
}

/// @brief Play the correct visual 
static void flashCorrect(void) {
    static const uint16_t notes[]     = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5 };
    static const uint8_t durations[] = { 150,     150,     150,     200 };
    playMelodyWithFlash(notes, durations, 4, greenColor);
}

/// @brief Play the wrong visual 
static void flashWrong(void) {
    static const uint16_t notes[]     = { NOTE_E4, NOTE_C4 };
    static const uint8_t durations[] = { 200,     250 };
    playMelodyWithFlash(notes, durations, 2, redColor);
}
