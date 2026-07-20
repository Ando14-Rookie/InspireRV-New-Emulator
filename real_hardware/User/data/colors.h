/// @brief This file contains the definition of color_256 struct and some preset color
/// values
#pragma once
#include "buttons.h"

#include <stdint.h>

/// @brief NOT FOLLOWING THE R,G,B ORDER!!!
/// See here: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
typedef struct color_256 {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} color_t;

/// @brief buffer
extern color_t led_array[NUM_LEDS];

// clang-format off
extern const color_t colors[NUM_LEDS];

// clang-format on

extern const uint16_t num_colors;

color_t color_divide(color_t color, uint8_t divider);

extern uint8_t ledDivisor;

#define smaller(x, y) ((x) < (y) ? (x) : (y))

void set_color(uint8_t led, color_t color, uint8_t ledDivisor);

void set_color_no_div(uint8_t led, color_t color);

/** 
 * @brief Fill the whole LED matrix with desired  color and brightness level
 * @param color The color for each LED
 * 
 **/
void fill_color(color_t color);

void clear(void);

/// @brief Handle the initial `RV logo screen` in the beginning of InspireRV 
void fill_logo(void);
