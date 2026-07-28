#ifndef _BINARY_GAME_H
#define _BINARY_GAME_H

#include <stdlib.h>
#include <time.h>
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
#include "./random_number_canvas.h"
#include "../data/buttons.h"
#include "../data/colors.h" 

#define defaultLogoColor onColorBlue // Blue

#define normalColor onColorPurple // Purple
#define selectedColor onColorYellow // Yellow
#define binaryGamereturnColor solidColorRed // Red
#define confirmColor onColorGreen // Green

/// @brief Handle the logic of the whole binary game
extern void initBinaryGame(void);

#endif
