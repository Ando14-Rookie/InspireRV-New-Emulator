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


extern const color_t onColorGreen;
extern const color_t onColorBlue;
extern const color_t offColor;
extern const color_t pointerColor;
extern const color_t solidColorRed;
extern const color_t lightColorRed;
/// @brief Buffer color data for 8x8 LED Matrix
extern color_t led_array[NUM_BUTTONS];

/// @brief The real saved color for 8x8 LED Matrix
extern color_t savedColor[NUM_BUTTONS];

/// @brief The real saved coding pages for 8x8 LED Matrix (for coding_space.c)
extern color_t savedCodingPages[NUM_BUTTONS];

// clang-format off
extern const color_t colors[NUM_BUTTONS];

// clang-format on

extern const uint16_t num_colors;

// All static inline function needs to be defined in the .h file

// To scale brightness down without changing the basic color
color_t color_divide(color_t color, uint8_t divider);

// If true, u get "x", otherwise, u get "y"
#define smaller(x, y) ((x) < (y) ? (x) : (y))
void set_color(uint8_t led, color_t color);

// Fill each LED with color
void fill_color(color_t color);

// Clear emulator screen
void clear(void);


#endif