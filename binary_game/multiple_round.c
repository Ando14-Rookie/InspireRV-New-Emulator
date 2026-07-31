#include "multiple_round.h"

enum RoundStatus roundStatus[5] = {
    ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE};

int8_t roundEntered[5] = {0};

const uint8_t overflowLogo[6][5] = {
    [0] = {0b10000111, 0b10011101, 0b11011101, 0b10100101, 0b11000111},
    [1] = {0b10000010, 0b10011110, 0b11011010, 0b10100010, 0b11000111},
    [2] = {0b10000111, 0b10011001, 0b11011111, 0b10100100, 0b11000111},
    [3] = {0b10000111, 0b10011001, 0b11011111, 0b10100001, 0b11000111},
    [4] = {0b10000011, 0b10011101, 0b11011111, 0b10100001, 0b11000001},
    [5] = {0b10000111, 0b10011100, 0b11011111, 0b10100001, 0b11000110}};

void renderGameRounds(void) {
    for (int8_t col = 4; col >= 0; col--) {
        // flip to match physical LED ordering
        int ledCol = 4 - col; // Used for roundStatus & LED in InspireRV

        // Columns 0-4 is always blue (default state)
        if (roundStatus[ledCol] == ROUND_IDLE) {
            // Default state = blue color
            setColorLEDScaled(ledCol, onColorBlue, brightnessDivisor);
            printf("Round %d is default. \n", ledCol);
        }
        else if (roundStatus[ledCol] == ROUND_CORRECT) {
            // answer is correct the 1st time answered
            setColorLEDScaled(ledCol, orangeColor, brightnessDivisor);
            printf("Round %d is correct.\n", ledCol);
        }
        else if ((roundStatus[ledCol] == ROUND_WRONG) ||
                 (roundStatus[ledCol] == ROUND_RETRY)) {
            // answer is wrong the 1st time answered even after retry
            setColorLEDScaled(ledCol, solidColorRed, brightnessDivisor);
            printf("Round %d is wrong/retry.\n", ledCol);
        }
    }
}

void handleGameRounds(bool answerCorrect, uint8_t roundIndex) {
    // currentRound happens to be the LED index for each round

    // Change roundStatus in current round if roundEntered current is also 0
    if (currentPage != PAINTING_SPACE && roundEntered[roundIndex] == 0 &&
        (roundStatus[roundIndex] == ROUND_IDLE)) {
        switch (answerCorrect) {
            case true:
                roundStatus[roundIndex] = ROUND_CORRECT;
                // Ensure that no matter right or wrong, need to show that the 1st time
                // entered round has occured
                roundEntered[roundIndex] = 1;

                // Allow to go to next round
                if (currentPage == BINARY_GAME) {
                    currentGame = BINARY_GAME_CONTINUE;
                }
                else if (currentPage == ADDITION_GAME) {
                    currentGame = ADDITION_GAME_CONTINUE;
                }
                setColorLEDScaled(roundIndex, orangeColor, brightnessDivisor);
                printf("I got correct answer the 1st time in round %d.\n", roundIndex);
                break;
            case false:
                roundStatus[roundIndex] = ROUND_RETRY;
                // Ensure that no matter right or wrong, need to show that the 1st time
                // entered round has occured
                roundEntered[roundIndex] = 1;
                setColorLEDScaled(roundIndex, magentaColor, brightnessDivisor);
                printf("Try again, I got wrong answer the 1st time in round %d.\n",
                    roundIndex);
                break;
        }
    }
    else if (currentPage != PAINTING_SPACE && roundEntered[roundIndex] == 1 &&
             (roundStatus[roundIndex] == ROUND_RETRY)) {
        switch (answerCorrect) {
            case true:
                roundStatus[roundIndex] = ROUND_WRONG;
                printf("Finally, this time I got the right answer during round %d.\n",
                    roundIndex);
                // Allow to go to next round
                if (currentPage == BINARY_GAME) {
                    currentGame = BINARY_GAME_CONTINUE;
                }
                else if (currentPage == ADDITION_GAME) {
                    currentGame = ADDITION_GAME_CONTINUE;
                }
                break;
            case false:
                // Keep the roundStatus[roundIndex], roundEntered[roundIndex] the same way
                break;
        }
    }
}

void renderResultsGraph(void) {
    // Will be used to display the 5-rounds result from leftmost to right
    uint8_t posRowSix = 6 * horizontalButtons + 7;
    uint8_t posRowSeven = 7 * horizontalButtons + 7;

    static const uint8_t trophyLogo[5] = {
        0b01111110, 0b10111101, 0b01111110, 0b00011000, 0b00111100};

    fill_color(offColor);

    // Run each round; Start with round 0 and LED at column 7
    for (uint8_t round = 0; round < 5; round++) {
        color_t dotColor = (roundStatus[round] == ROUND_CORRECT) ? confirmColorCorrect
                                                                 : confirmColorWrong;
        // Row 7 & 6, columns 7-3
        setColorLEDScaled((posRowSeven - round), dotColor, brightnessDivisor);
        setColorLEDScaled((posRowSix - round), dotColor, brightnessDivisor);
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
                setColorLEDScaled(idx, onColorYellow, brightnessDivisor);
            }
        }
    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void renderOverflowGraph(void) {
    // Will be used to display the 5-rounds result from leftmost to right
    uint8_t posRowSix = 6 * horizontalButtons + 7;
    uint8_t posRowSeven = 7 * horizontalButtons + 7;

    fill_color(offColor);

    // Run each round; Start with round 0 and LED at column 7
    for (uint8_t round = 0; round < 5; round++) {
        color_t dotColor = (roundStatus[round] == ROUND_CORRECT) ? confirmColorCorrect
                                                                 : confirmColorWrong;
        // Row 7 & 6, columns 7-3
        setColorLEDScaled((posRowSeven - round), dotColor, brightnessDivisor);
        setColorLEDScaled((posRowSix - round), dotColor, brightnessDivisor);
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
                setColorLEDScaled(idx,
                    (ledCol == 2 || ledCol == 1 || ledCol == 0) ? onColorBlue
                                                                : solidColorRed,
                    brightnessDivisor);
            }
        }
    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void flashGameComplete(void) {
    static const uint8_t notes[] = {
        NOTE_GS5, NOTE_D6, NOTE_A5, NOTE_FS6, NOTE_D6, NOTE_FS6};
    static const uint16_t durations[] = {150, 150, 150, 300, 150, 600};
    playMelodyWithFlash(notes, durations, 6, confirmColorCorrect);
}