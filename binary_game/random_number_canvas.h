#ifndef _RANDOM_NUMBER_CANVAS_H
#define _RANDOM_NUMBER_CANVAS_H

#include <stdint.h>

/** 
 * Columns 7(controlRow[0]) = quit button (red)
 * Columns 0(controlRow[7]) = confirm button (green)
 * 
 * Columns 2-5 (userInputRow[2] until userInputRow[5]) are the 4 binary input LEDs (center of the row).
 * `Bit order`: col5 = bit3 (MSB), col4 = bit2, col3 = bit1, col2 = bit0 (LSB)
 * `-1` means it can't do anything
 * 
 **/ 
extern int8_t rowOneHandle[8];

/// @brief slotLogo[`which number to show`][`row`][`col`]
extern const int slotLogo[16][5][8];

#endif