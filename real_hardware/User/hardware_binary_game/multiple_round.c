#include "multiple_round.h"

RoundStatus roundStatus[5] = {
    ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE};

int8_t roundEntered[5] = {0};

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
                currentGame = BINARY_GAME_CONTINUE;
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
                currentGame = BINARY_GAME_CONTINUE;
                break;
            case false:
                // Keep the roundStatus[roundIndex], roundEntered[roundIndex] the same way
                break;
        }
    }
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
    playMelodyWithFlash(notes, durations, 6, greenColor);
}