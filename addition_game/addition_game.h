#ifndef _ADDITION_GAME_H
#define _ADDITION_GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../data/colors.h"
#include "../data/music.h"
#include "../ch32v003fun/ws2812b_simple.h"
//Round Status, State, Function taken from the binary game C file
#include "../binary_game/multiple_round.h"

#define normalColor onColorPurple        // Purple
#define selectedColor onColorYellow      // Yellow
#define overflowColor solidColorRed  // Red
#define confirmColor onColorGreen        // Purple

typedef enum {
    ADDITION_GAME_IDLE = 0,
    ADDITION_INPUT_CONFIRM,
    ADDITION_GAME_CONTINUE,
    ADDITION_GAME_GRAPH = 3
} AdditionState;

// Global variable
extern AdditionState currentAddGame;
// Check how many times the question in each round causes an overflow
extern uint8_t overflowOccur;

/// @brief Handle the logic of the whole addition game
void initAdditionGame(void);

#endif