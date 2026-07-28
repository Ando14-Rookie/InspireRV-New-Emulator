#ifndef _BINARY_GAME_H
#define _BINARY_GAME_H

#include "../data/buttons.h"
#include "../data/colors.h"
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
#include "./multiple_round.h"
#include "./random_number_canvas.h"

#include <stdlib.h>
#include <time.h>

#define defaultLogoColor onColorBlue // Blue

#define normalColor onColorPurple        // Purple
#define selectedColor onColorYellow      // Yellow
#define confirmColorWrong solidColorRed  // Red
#define confirmColorCorrect onColorGreen // Green

typedef enum {
    BINARY_GAME_IDLE = 0,
    GAME_INPUT_CONFIRM,
    BINARY_GAME_CONTINUE,
    BINARY_GAME_GRAPH = 3
} GameState;

// Global variable
extern GameState currentGame;

/// @brief Handle the logic of the whole binary game
extern void initBinaryGame(void);

/**
 * @brief Create short visual to show if user answer is correct or false
 * @param notes List of notes to use
 * @param duration How long should each note/blinking behaviour last
 * @param len How many times should it loops
 * @param color What color should be used for the blinking
 **/
void playMelodyWithFlash(
    const uint8_t * notes, const uint16_t * durations, uint8_t len, color_t color);

#endif
