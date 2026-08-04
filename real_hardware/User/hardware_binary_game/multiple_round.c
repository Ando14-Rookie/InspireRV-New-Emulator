#include "multiple_round.h"

// Function prototype declaration
static inline void renderChooseGameMode(uint8_t brightnessDivisor);

RoundStatus roundStatus[5] = {
    ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE};

int8_t roundEntered[5] = {0};

GameplayModeHW playGameMode = ADDITION_GAME;

const uint8_t overflowLogo[6][5] = {
    [0] = {0b10000111, 0b10011101, 0b11011101, 0b10100101, 0b11000111},
    [1] = {0b10000010, 0b10011110, 0b11011010, 0b10100010, 0b11000111},
    [2] = {0b10000111, 0b10011001, 0b11011111, 0b10100100, 0b11000111},
    [3] = {0b10000111, 0b10011001, 0b11011111, 0b10100001, 0b11000111},
    [4] = {0b10000011, 0b10011101, 0b11011111, 0b10100001, 0b11000001},
    [5] = {0b10000111, 0b10011100, 0b11011111, 0b10100001, 0b11000110}};

// Store the gameplay screen here
static const char* screenPattern[8] = {
    "12120330",
    "21213333",
    "12123333",
    "21210330",
    "00000000",
    "02200330",
    "22223333",
    "00000000"
};

uint16_t chooseGamemodeScreen[7] = {0};

void chooseGameMode(uint8_t brightnessDivisor) {
    // Will stay in this mode until user presses confirm button to choose the game mode
    bool chooseMode = false;
    // Check if the enter has been pressed or not
    ChooseEnterPressed enterPressed = CHOOSE_GAME_NOT_ENTERED;
    // Store which button is being pressed
    int8_t button = noButtonPressed;
    // Print the emulator screen for the first time for this function
    renderChooseGameMode(brightnessDivisor);

    while(!chooseMode){

        if (JOY_9_pressed()) {
            chooseMode = true;
            break;
        }

        //  Repeatedly polls/checks the button matrix
        button = matrix_pressed_two();

        if(button != noButtonPressed){
            // If user has pressed the confirm button, then choose the game mode
            if((button >= 8 && button <= 11) || button == 17 || button == 18){
                playGameMode = ADDITION_GAME;
                chooseMode = true;
            }
            else if((button >= 12 && button <= 15) || button == 21 || button == 22){
                playGameMode = BINARY_GAME;
                chooseMode = true;
            }
            else{
                // Print the emulator screen for the first time for this function
                renderChooseGameMode(brightnessDivisor);
            }
        }

    }

    //Reset values
    chooseMode = false;
    enterPressed = CHOOSE_GAME_NOT_ENTERED;
    button = noButtonPressed;
    // Rerender canvas using real current data
    flushCanvas();
}

static inline void renderChooseGameMode(uint8_t brightnessDivisor) {
    // Clear the screen first
    fill_color(offColor);
    // Render the game mode selection screen
    for (uint8_t arrayRow = 0; arrayRow <= 7; arrayRow++) {
        int8_t ledRow = 7 - arrayRow; // top (7) down to row 3
        for (uint8_t arrayCol = 0; arrayCol < 8; arrayCol++) {
            int ledCol = 7 - arrayCol;
            int idx = ledRow * 8 + ledCol;
            // '0'-'3' as ASCII characters need to be converted to actual
            // numbers 0-3. Subtracting the character '0' does this:
            // e.g. '2' - '0' = 50 - 48 = 2
            uint8_t state = screenPattern[arrayRow][arrayCol] - '0';
            switch(state){
                case 0:
                    set_color(idx, offColor, brightnessDivisor);
                    break;
                case 1:
                    set_color(idx, blueColor, brightnessDivisor);
                    break;
                case 2:
                    set_color(idx, cyanColor, brightnessDivisor);
                    break;
                case 3:
                    set_color(idx, redColor, brightnessDivisor);
                    break;
            }
        }
    }

    // Print the emulator screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void renderGameRounds(uint8_t brightnessDivisor) {
    for (int8_t col = 4; col >= 0; col--) {
        // flip to match physical LED ordering
        int ledCol = 4 - col; // Used for roundStatus & LED in InspireRV

        // Columns 0-4 is always blue (default state)
        if (roundStatus[ledCol] == ROUND_IDLE) {
            // Default state = blue color
            set_color(ledCol, blueColor, brightnessDivisor);
            // printf("Round %d is default. \n", ledCol);
        }
        else if (roundStatus[ledCol] == ROUND_CORRECT) {
            // answer is correct the 1st time answered
            set_color(ledCol, orangeColor, brightnessDivisor);
            // printf("Round %d is correct.\n", ledCol);
        }
        else if ((roundStatus[ledCol] == ROUND_WRONG) ||
                 (roundStatus[ledCol] == ROUND_RETRY)) {
            // answer is wrong the 1st time answered even after retry
            set_color(ledCol, redColor, brightnessDivisor);
            // printf("Round %d is wrong/retry.\n", ledCol);
        }
    }
}

void handleGameRounds(bool answerCorrect, uint8_t roundIndex, uint8_t brightnessDivisor ) {
    // currentRound happens to be the LED index for each round

    // Change roundStatus in current round if roundEntered current is also 0
    if (roundEntered[roundIndex] == 0 &&
        (roundStatus[roundIndex] == ROUND_IDLE)) {
        switch (answerCorrect) {
            case true:
                roundStatus[roundIndex] = ROUND_CORRECT;
                // Ensure that no matter right or wrong, need to show that the 1st time
                // entered round has occured
                roundEntered[roundIndex] = 1;
                // Allow to go to next round
                if (playGameMode == BINARY_GAME) {
                    currentGame = BINARY_GAME_CONTINUE;
                }
                else if (playGameMode == ADDITION_GAME) {
                    currentAddGame = ADDITION_GAME_CONTINUE;
                }
                set_color(roundIndex, orangeColor, brightnessDivisor);
                // printf("I got correct answer the 1st time in round %d.\n", roundIndex);
                break;
            case false:
                roundStatus[roundIndex] = ROUND_RETRY;
                // Ensure that no matter right or wrong, need to show that the 1st time
                // entered round has occured
                roundEntered[roundIndex] = 1;
                set_color(roundIndex, magentaColor, brightnessDivisor);
                // printf("Try again, I got wrong answer the 1st time in round %d.\n",
                    // roundIndex);
                break;
        }
    }
    else if (roundEntered[roundIndex] == 1 &&
             (roundStatus[roundIndex] == ROUND_RETRY)) {
        switch (answerCorrect) {
            case true:
                roundStatus[roundIndex] = ROUND_WRONG;
                // printf("Finally, this time I got the right answer during round %d.\n",
                    // roundIndex);
                // Allow to go to next round
                if (playGameMode == BINARY_GAME) {
                    currentGame = BINARY_GAME_CONTINUE;
                }
                else if (playGameMode == ADDITION_GAME) {
                    currentAddGame = ADDITION_GAME_CONTINUE;
                }
                break;
            case false:
                // Keep the roundStatus[roundIndex], roundEntered[roundIndex] the same way
                break;
        }
    }
}

void renderOverflowGraph(uint8_t overflowOccur, uint8_t brightnessDivisor) {
    // Will be used to display the 5-rounds result from leftmost to right
    uint8_t posRowSix = 6 * horizontalButtons + 7;
    uint8_t posRowSeven = 7 * horizontalButtons + 7;

    fill_color(offColor);

    // Run each round; Start with round 0 and LED at column 7
    for (uint8_t round = 0; round < 5; round++) {
        color_t dotColor = (roundStatus[round] == ROUND_CORRECT) ? greenColor
                                                                 : redColor;
        // Row 7 & 6, columns 7-3
        set_color((posRowSeven - round), dotColor, brightnessDivisor);
        set_color((posRowSix - round), dotColor, brightnessDivisor);
    }

    // Create the overflow logo using yellow color
    for (int arrayRow = 0; arrayRow < 5; arrayRow++) {
        int ledRow = 4 - arrayRow; // top (7) down to row 3
        uint8_t rowBits = overflowLogo[overflowOccur][arrayRow];
        for (int arrayCol = 0; arrayCol < 8; arrayCol++) {
            int ledCol = 7 - arrayCol;
            int idx = ledRow * 8 + ledCol;
            // Bit 7 = column 0, so shift right by (7 - arrayCol) or ledCol
            int val = (rowBits >> (ledCol)) & 1;

            if (val == 1) {
                set_color(idx,
                    (ledCol == 2 || ledCol == 1 || ledCol == 0) ? blueColor
                                                                : redColor,
                    brightnessDivisor);
            }
        }
    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void renderResultsGraph(uint8_t brightnessDivisor) {
    // Will be used to display the 5-rounds result from leftmost to right
    uint8_t posRowSix = 6 * horizontalButtons + 7;
    uint8_t posRowSeven = 7 * horizontalButtons + 7;
    
    static const uint8_t trophyLogo[5] = {
        0b01111110, 0b10111101, 0b01111110, 0b00011000, 0b00111100
    }; 

    fill_color(offColor);

    // Run each round; Start with round 0 and LED at column 7
    for (uint8_t round = 0; round < 5; round++) {
        color_t dotColor = (roundStatus[round] == ROUND_CORRECT) ? greenColor : redColor;
        // Row 7 & 6, columns 7-3
        set_color((posRowSeven - round), dotColor, brightnessDivisor);  
        set_color((posRowSix - round), dotColor, brightnessDivisor);  
    }

    // Create the trophy logo using yellow color
    for (int arrayRow = 0; arrayRow < 5; arrayRow++) {
        int ledRow = 4 - arrayRow; // top (7) down to row 3
        uint8_t rowBits = trophyLogo[arrayRow];
        for (int arrayCol = 0; arrayCol < 8; arrayCol++) {
            int ledCol = 7 - arrayCol;
            int idx = ledRow * 8 + ledCol;
            // Bit 7 = column 0, so shift right by (7 - arrayCol) or ledCol
            int val = (rowBits >> (ledCol)) & 1;

            if (val == 1) {
                set_color(idx, yellowColor, brightnessDivisor);
            }
        }
    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void flashGameComplete(void) {
    static const uint16_t notes[] = {NOTE_GS4, NOTE_D5, NOTE_A4, NOTE_FS5, NOTE_D5, NOTE_FS5};
    static const uint8_t durations[] = {150, 150, 150, 300, 150, 600};
    playMelodyWithFlashHW(notes, durations, 6, greenColor);
}

uint32_t seedRandomFromHW(uint32_t nextRandom) {
    // Use CH32V003's free-running timer, then XOR with ADC noise as entropy source
    nextRandom = GPIO_analogRead(GPIO_Ain1_A1);
    if (nextRandom == 0)
        return nextRandom = 1; // avoid degenerate all-zero state
    else
        return nextRandom;
}

uint16_t getRandom0to15(uint32_t nextRandom) {
    // Extracts a 4-bit value from the middle-upper bits of the 32-bit state
    nextRandom = nextRandom * 1103515245 + 12345;
    return (nextRandom >> 16) & 0xF; // use upper bits, mask to 0-15
}