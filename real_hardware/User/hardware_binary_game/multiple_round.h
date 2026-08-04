#ifndef _MULTIPLE_ROUND_H
#define _MULTIPLE_ROUND_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../data/buttons.h"
#include "../data/colors.h"

#include "binary_game.h"
#include "addition_game.h"

// Status for the multiple round game
typedef uint8_t RoundStatus;
#define ROUND_IDLE    0
#define ROUND_CORRECT 1
#define ROUND_RETRY   2
#define ROUND_WRONG   3

// Status for what game is being played
typedef uint8_t GameplayModeHW;
#define CHOOSE_GAME    0
#define BINARY_GAME    1
#define ADDITION_GAME 2

// Store the gameplay mode here
extern GameplayModeHW playGameMode;

typedef uint8_t ChooseEnterPressed;
#define CHOOSE_GAME_NOT_ENTERED 0
#define CHOOSE_GAME_ENTERED 1

// Store the multiple round state here
extern RoundStatus roundStatus[5];

// If it is the 1st time in that round, do +1; If round is already 1, then it can't edit
// roundStatus anymore
extern int8_t roundEntered[5];

/// @brief Logo to be shown to indicate how many question causes overflow from each round
extern const uint8_t overflowLogo[6][5];

/// @brief Let user choose which game he wants to play. Either `Binary Game` or `Addition Game`.
/// @param brightnessDivisor Set each LED how bright it needs to be
void chooseGameMode(uint8_t brightnessDivisor);

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
 * @brief Create random seed everytime program runs/function used
 * @param nextRandom Unpredictable data taken from 2 random sources from hardware.
 * @return New random number
 * */ 
uint32_t seedRandomFromHW(uint32_t nextRandom);

/** 
 * @brief Take upperBits from `nextRandom` then extract the upper bits `resulting 0 to 15`
 * @param nextRandom Unpredictable data taken from 2 random sources from hardware.
 * @return Value from 0 to 15
 * */ 
uint16_t getRandom0to15(uint32_t nextRandom);

/**
 * @brief Render the graph shown in the final scene after playing 5 game of round
 * for `ADDITION_GAME`
 * @param overflowOccur Total number of how many times each round causes an overflow in the `4-bit binary addition` game
 * @param brightnessDivisor Set each LED how bright it needs to be
 **/
void renderOverflowGraph(uint8_t overflowOccur, uint8_t brightnessDivisor);

/**
 * @brief Render the graph shown in the final scene after playing 5 game of round
 * @param brightnessDivisor Set each LED how bright it needs to be
 **/
void renderResultsGraph(uint8_t brightnessDivisor);

/// @brief Function that handle the graph rendering after playing 
void flashGameComplete(void);

#endif