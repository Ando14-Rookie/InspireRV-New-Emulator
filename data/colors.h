/// @brief This file contains the definition of color_256 struct and some preset color
/// values
// #pragma once
#ifndef COLORS_H
#define COLORS_H
#include <stdint.h>

//Declare that this color_t will be defined later
typedef struct color_t color_t;

typedef struct color_t {
    uint8_t g;
    uint8_t r;
    uint8_t b;
};

#include "buttons.h"
#include "../coding_space/coding_space.h"

/// @brief NOT FOLLOWING THE R,G,B ORDER!!!
/// See here: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
// Send the high value data first which is G7-G0

// Needs to be able to adapt to different brightness level
extern color_t onColorGreen;
extern color_t onColorYellow;
extern color_t onColorPurple;
extern color_t onColorBlue;

extern const color_t fixedColorGreen;
extern const color_t offColor;
extern const color_t pointerColor;
extern const color_t solidColorRed;
extern const color_t lightColorRed;

/// @brief Buffer color data for 8x8 LED Matrix
extern color_t led_array[NUM_BUTTONS];

/// @brief Remember which LEDs was on previously
extern color_t previousLedSnapshot[NUM_LEDS];

// TO BE MOVED to painting_space.h
/// @brief The real saved color for 8x8 LED Matrix for PAINTING_SPACE
extern color_t savedColor[NUM_BUTTONS];

/// @brief The real saved coding pages for 8x8 LED Matrix (for coding_space.c)
extern color_t savedCodingPages[NUM_BUTTONS];

// clang-format off
extern const color_t colors[NUM_BUTTONS];

// clang-format on

extern const uint16_t num_colors;

// All static inline function needs to be defined in the .h file

/** 
 * @brief Determine the brightness of all LED in both `coding and painting space`
 * It changes only the `led_array`
 * @param storage Which storage of the LED are we referring to
 * @param color RGB color of selected LED
 * @param divisor Higher divisor value means dimmer color
**/
void setColorLEDScaled(uint8_t storage, color_t color, uint8_t divisor);

/** 
 * @brief Change brightness used of all type of color such as `scaledForeground`, `scaledBackground`, 
 * `savedColor`, etc.
 * 
 * @param storageArray Where the LED index is stored
 * @param index Which position the LED is in
 * @param color New RGB color for selected LED
 * @param divisor Higher divisor value means dimmer color
**/
void changeColorScaled(color_t *storageArray, uint8_t index, color_t newColor, uint8_t divisor);

/** 
 * @brief Determine the brightness of color in both `coding and painting space`
 * It is only used to initialize the color of scaledForeground and scaledBackground color.
 * @param color RGB color of selected color
 * @param divisor Higher divisor value means dimmer color
 * 
 * @return Return the brightness scaled version of that color
**/
color_t initColorScaled(color_t color, uint8_t divisor);

// To scale brightness down without changing the basic color
color_t color_divide(color_t color, uint8_t divider);

// If true, u get "x", otherwise, u get "y"
#define smaller(x, y) ((x) < (y) ? (x) : (y))

/** 
 * @brief Set Color normally
 * @param led Which LED position to be setup
 * @param color What color to be stored
 **/ 
void set_color(uint8_t led, color_t color);

// Fill each LED with color
void fill_color(color_t color);

// Clear emulator screen
void clear(void);


#endif