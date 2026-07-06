#pragma once

#include "colors.h"
// #include "funconfig.h"

#define font_width 3
#define font_height 5
#define font_size (font_width * font_height)

// clang-format off

// extern: will be define somewhere else
// inline: insert a block of code, like small function, into bigger main function
// static: only private to the defined file, unless "static inline", each file gets its own copy
extern const uint8_t font_1[font_size];

extern const uint8_t font_2[font_size];

extern const uint8_t font_3[font_size];

extern const uint8_t font_4[font_size];

extern const uint8_t font_5[font_size];

extern const uint8_t font_6[font_size];

extern const uint8_t font_7[font_size];

extern const uint8_t font_8[font_size];

extern const uint8_t font_9[font_size];

extern const uint8_t font_0[font_size];

extern const uint8_t font_X[font_size];

extern const uint8_t font_D[font_size];

// clang-format on

const uint8_t * font_list[] = {font_0, font_1, font_2, font_3, font_4, font_5, font_6,
    font_7, font_8, font_9, font_X, font_D};

extern const int num_fonts;

extern void font_draw(const uint8_t font[], color_t color, int startpos);