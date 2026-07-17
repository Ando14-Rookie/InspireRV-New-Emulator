// make the header’s contents be included only once in the same compilation unit
#ifndef EXTRA_FUNCTION_H
#define EXTRA_FUNCTION_H

#include "../../data/buttons.h"
#include "../../data/colors.h"
#include "../../painting_space/paint_save_space.h"
#include "../../coding_space/code_save_space.h"
#include "../../coding_space/coding_space.h" 
#include "./emulator_driver/emulator_driver.h"
#include "../../emulator-screen/brightness_control.h"
#include "../../binary_game/binary_game.h"

#include "../emulator/adriel_2026_work/system_window_mac.h"

#include <pthread.h>
#include <stdbool.h> // Required for the bool type, true, and false
#include <stdio.h>
#include <string.h>

// #include "../emulator-screen/led_matrix_screen.h"

extern int currentKey;

/// @brief Determine what your previous page state is
extern PageState prevPageState;

/**
 * @brief Handle the key pressed logic for the 9 buttons of the InspireRV
 * */
extern void handleButtonFunction(void);

/**
 * @brief Check if number key `9` has been pressed as this will be used to stop
 * `makeScreenEmulator` function
 * */
extern inline bool isRunning(void);

/**
 * @brief Converts a 1D LED index (0–63) into a 2D row and column on the 8×8 grid
 *
 * @param idx position of LED
 * @param row points LED to which row
 * @param col points LED to which column
 * */
static inline void index_to_rc(int idx, int * row, int * col);

/**
 * @brief The reverse of index_to_rc which is to convert a 2D row/col back to a 1D index.
 * @param row which row the LED is in
 * @param col which column the LED is in
 * */
static inline int rc_to_index(int row, int col);

/**
 * @brief Checks if two colors are identical by comparing all three channels, i.e., r, g,
 * b.
 * @param a first LED to be compared
 * @param b second LED to be compared
 * */
static inline bool color_equals(color_t a, color_t b);
#endif
