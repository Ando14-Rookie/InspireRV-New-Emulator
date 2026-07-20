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
    // Get random seed
    seedRandomFromHW();
    // Randomize number and get the value from 0 to 15
    uint8_t questionIndex = getRandom0to15();

    // #ifdef DEBUG_VERBOSE
    printf("Question value is %d \n", questionIndex);
    // #endif

    // Display binary question first time
    displayBinaryQuestionHW(questionIndex, brightnessDivisor);

    while (1) {

        //  Repeatedly polls/checks the button matrix
        button = matrix_pressed_two();

        // Check if any button is pressed
        // Handle the user input
        if (button != noButtonPressed) {
            // Keep showing everytime user select/press something, unless quitting the
            // game
            displayBinaryQuestionHW(questionIndex, brightnessDivisor);

            // Handle quit
            if (button == 15) {
                break;
            }

            // Handle confirm
            if (button == 8) {
                // Check user input with the corresponding `nextRandom` binary question
            }

            // Handle user input
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
                set_color(idx, (color_t){100, 100, 255}, brightnessDivisor);
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
                set_color(idx, (color_t){255, 255, 100},
                    brightnessDivisor); // selected = yellow
                break;
            case -1:
                set_color(idx, (color_t){100, 255, 100},
                    brightnessDivisor); // quit = red (per your comment)
                break;
            case -2:
                set_color(idx, (color_t){255, 0, 0},
                    brightnessDivisor); // confirm = green (per your comment)
                break;
            case 3:
                set_color(
                    idx, (color_t){0, 0, 0}, brightnessDivisor); // unused LEDs = off
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