#ifndef _MULTIPLE_ROUND_H
#define _MULTIPLE_ROUND_H

#include <stdio.h>
#include <stdint.h>
#include "../data/buttons.h"
#include "../data/music.h"
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

// Store the overflow state
extern const uint8_t overflowLogo[6][5];

// If it is the 1st time in that round, do +1; If round is already 1, then it can't edit
// roundStatus anymore
extern int8_t roundEntered[5];

// Status for what game is being played
enum GameplayMode {
    CHOOSE_GAME, // 0
    BINARY_GAME, // 1
    ADDITION_GAME // 2
};

// Store the gameplay mode
extern enum GameplayMode playGameMode;

typedef uint8_t ChooseEnterPressed;
#define CHOOSE_GAME_NOT_ENTERED 0
#define CHOOSE_GAME_ENTERED 1

/// @brief Let user choose which game he wants to play. Either `Binary Game` or `Addition Game`.
void chooseGameMode(void);

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
 * for `BINARY_GAME`
 **/
void renderResultsGraph(void);

/**
 * @brief Render the graph shown in the final scene after playing 5 game of round
 * for `ADDITION_GAME`
 **/
void renderOverflowGraph(void);

/// @brief Function that handle the graph rendering after playing 
void flashGameComplete(void);

#endif