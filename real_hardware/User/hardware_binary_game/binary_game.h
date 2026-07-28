#ifndef _BINARY_GAME_H
#define _BINARY_GAME_H

// 1. Declare hardware configurations required by driver.h
#ifndef WS2812BSIMPLE_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#endif

#ifndef INTERNAL_INSPIRE_MATRIX
#define INTERNAL_INSPIRE_MATRIX 1
#endif

// 2. Safely include your hardware peripheral library 

#ifdef abs 
#undef abs
#endif
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
#include <stdbool.h>
#include "../data/colors.h"
#include "../data/music.h"
#include "multiple_round.h"
#include "../ch32v003fun/driver.h"
#include "../ch32v003fun/ws2812b_simple.h"

#define noButtonPressed -1
#define LED_PINS GPIOA, 2
#define defaultLogoColor onColorBlue // Blue

typedef enum { BINARY_GAME_IDLE = 0, GAME_INPUT_CONFIRM, BINARY_GAME_CONTINUE } GameState;

// Global variable
// Status for the multiple round game
typedef uint8_t GameStatus;
#define BINARY_GAME_IDLE 0
#define GAME_INPUT_CONFIRM 1
#define BINARY_GAME_CONTINUE 2
#define BINARY_GAME_GRAPH 3

#undef DEBUG_VERBOSE 1

extern GameState currentGame;

/** 
 * @brief  Show and handle the Binary Game 
 * @param brightnessDivisor Brightness level to be used in each LED
 **/ 
extern void renderBinaryGameHW(uint8_t brightnessDivisor);

/**
 * @brief Create short visual to show if user answer is correct or false
 * @param notes List of notes to use
 * @param duration How long should each note/blinking behaviour last
 * @param len How many times should it loops
 * @param color What color should be used for the blinking
 **/
void playMelodyWithFlash(
    const uint16_t * notes, const uint8_t * durations, uint8_t len, color_t color);

#endif