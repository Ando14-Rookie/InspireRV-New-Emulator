#ifndef _MULTIPLE_ROUND_H
#define _MULTIPLE_ROUND_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../data/buttons.h"
#include "../data/colors.h"

#include "binary_game.h"

// Status for the multiple round game
typedef uint8_t RoundStatus;
#define ROUND_IDLE    0
#define ROUND_CORRECT 1
#define ROUND_RETRY   2
#define ROUND_WRONG   3

// Store the multiple round state here
extern RoundStatus roundStatus[5];

// If it is the 1st time in that round, do +1; If round is already 1, then it can't edit
// roundStatus anymore
extern int8_t roundEntered[5];
/**
 * @brief Call this everytime emulator screen is printed. This print the user input
 * , confirm button and quit button
 * @param brightnessDivisor Set each LED how bright it needs to be
 **/
void renderGameRounds(uint8_t brightnessDivisor);

/**
 * @brief Handle the logic of the multiple game rounds
 * @param answerCorrect True if current round answer is correct; Otherwise, false
 * @param currentRound The current number of the round; It also happens to point to which LED should it change/turn
 * @param brightnessDivisor Set each LED how bright it needs to be
 **/
void handleGameRounds(bool answerCorrect, uint8_t currentRound, uint8_t brightnessDivisor);

/**
 * @brief Render the graph shown in the final scene after playing 5 game of round
 * @param brightnessDivisor Set each LED how bright it needs to be
 **/
void renderResultsGraph(uint8_t brightnessDivisor);

/// @brief Function that handle the graph rendering after playing 
void flashGameComplete(void);

#endif