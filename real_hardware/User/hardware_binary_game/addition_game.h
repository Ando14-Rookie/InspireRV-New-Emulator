#ifndef _ADDITION_GAME_H
#define _ADDITION_GAME_H

#define DEBUG_TEST 1

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../data/colors.h"
#include "../data/music.h"
#include "../ch32v003fun/ws2812b_simple.h"
//Round Status, State, Function taken from the binary game C file
#include "./multiple_round.h"

#define normalColor onColorPurple        // Purple
#define selectedColor onColorYellow      // Yellow
#define overflowColor solidColorRed  // Red
#define confirmColor onColorGreen        // Purple

typedef uint8_t AdditionState;
#define ADDITION_GAME_IDLE 0
#define ADDITION_INPUT_CONFIRM 1
#define ADDITION_GAME_CONTINUE 2
#define ADDITION_GAME_GRAPH 3

// Global variable
extern AdditionState currentAddGame;
// Check how many times the question in each round causes an overflow
extern uint8_t overflowOccur;

/// @brief Handle the logic of the whole addition game
void initAdditionGameHW(uint8_t brightnessDivisor);

#endif