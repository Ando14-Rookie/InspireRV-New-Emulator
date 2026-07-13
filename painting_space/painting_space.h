#ifndef PAINTING_SPACE_H
#define PAINTING_SPACE_H
#define WS2812BSIMPLE_IMPLEMENTATION
#include "../data/colors.h" 
#include "../data/buttons.h" // Needed to update which LED to turn on
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
// #include "..\ch32v003fun\ws2812b_simple.h"
#include <stdio.h>
#include <stdint.h>

#define LED_PINS GPIOC, 2

// Default LED color (can be changed)
extern color_t foregroundColor; // Green
extern color_t backgroundColor; // Blue
extern color_t scaledForeground; //Bridghtness scaled
extern color_t scaledBackground; //Bridghtness caled
extern color_t tempColor; // Container for bucket fill feature
extern color_t slotColor; 
extern color_t returnColor; 
#define defaultColor offColor // Black
#define confirmColor onColorGreen // Green

// Let user choose which color they want for the foreground & background color
extern color_t colorfulMap[NUM_BUTTONS];

// Check if forgeround color is being used
extern bool pickingForeground;

/** 
 * @brief Initialize both the scaled foreground and background color
**/
void initScaledForeBackColors(void);

/** 
 * @brief Create smooth, high-resolution transitions between LED color and brightness
 * @return Return value = the blended result between a and b
 * @param initVal starting value
 * @param endVal ending value
 * @param row which step you are on, from 0 to 7
 **/
static uint8_t lerp_u8(uint8_t initVal, uint8_t endVal, uint8_t row);

/**
 * @brief The core flood fill algorithm that spreads outward and paints every connected
 * LED
 * @param startIndex The first selected LED by user where it will start
 * @param fillColor Color used for bucket fill feature
 * */
extern void bucketFill(int startIndex, color_t fillColor);

/**
 * @brief Contain the full logic of bucket fill with selecting color feature
 **/
extern void fullBucketFillLogic(void);

/** 
 * @brief Create the colorfulMap that user can choose
 **/
extern void initColorfulMap(void);

/**
 * @brief Logic function for choosing the foreground and background color
 * */
extern void chooseColorForeBack(void);

/**
 * @brief Set color in `led_array` and check the color map in `ledCondition` to update whether it has been pressed 2 times
 * 
 * @param led Position of LED
 */
void updateLEDColor(uint8_t led);

/**
 * @brief Update foreground color in Painting Space
 * 
 * @param foregroundLEDColor Type of colour to be set from the chosen. 
 * Value is taken from a list of color in `colorfulLED` variable
 */
void updateForegroundColor(color_t foregroundLEDColor);

/**
 * @brief Update background color in Painting Space
 * 
 * @param backgroundLEDColor Type of colour to be set from the chosen. 
 * Value is taken from a list of color in `colorfulLED` variable
 */
void updateBackgroundColor(color_t backgroundLEDColor);

#endif
