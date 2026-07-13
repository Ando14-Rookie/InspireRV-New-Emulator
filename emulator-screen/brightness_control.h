#ifndef BRIGHTNESS_CONTROL_H
#define BRIGHTNESS_CONTROL_H

#include "../data/colors.h"
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
#include "../coding_space/coding_space.h"
#include "../painting_space/painting_space.h"

/// @brief Brightness Intensity; 1: Full brightness, higher = dimmer
extern uint8_t brightnessDivisor;

/** 
 * @brief Create a row of green LED in the end of the row with different brightness
**/
void createBrightnessSelectionScreen(void);

/// @brief Render the page to choose brightness level, used in `Button 2: BRIGHTNESS CONTROL` 
void renderBrightnessSelectScreen(void);

#endif