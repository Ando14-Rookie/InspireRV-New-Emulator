#ifndef _MULTIPLE_ROUND_H
#define _MULTIPLE_ROUND_H

#include <stdio.h>
#include <stdint.h>
#include "../data/buttons.h"
#include "../data/colors.h"

// Status for the multiple round game
enum RoundStatus {
    ROUND_IDLE,    // 1
    ROUND_CORRECT,  // 2
    ROUND_RETRY, // 3
    ROUND_WRONG
};

// Store the multiple round state here
extern enum RoundStatus roundStatus[5];

// If it is the 1st time in that round, do +1; If round is already 1, then it can't edit
// roundStatus anymore
extern int8_t roundEntered[5];
/**
 * @brief Call this everytime emulator screen is printed. This print the user input
 * , confirm button and quit button
 **/
void renderGameRounds(void);

/**
 * @brief Handle the logic of the multiple game rounds
 * @param answerCorrect True if current round answer is correct; Otherwise, false
 * @param currentRound The current number of the round; It also happens to point to which LED should it change/turn
 **/
void handleGameRounds(bool answerCorrect, uint8_t currentRound);

/**
 * @brief Render the graph shown in the final scene after playing 5 game of round
 **/
void renderResultsGraph(void);

/// @brief Function that handle the graph rendering after playing 
void flashGameComplete(void);

#endif