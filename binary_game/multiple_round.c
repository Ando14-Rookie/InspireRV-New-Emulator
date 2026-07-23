#include "multiple_round.h"

enum RoundStatus roundStatus[5] = {
    ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE, ROUND_IDLE};

int8_t roundEntered[5] = {0};

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
                currentGame = BINARY_GAME_CONTINUE;
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
                currentGame = BINARY_GAME_CONTINUE;
                break;
            case false:
                // Keep the roundStatus[roundIndex], roundEntered[roundIndex] the same way
                break;
        }
    }
}
