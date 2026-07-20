#pragma once

#include "colors.h"
#include "../funconfig.h"

#define font_width 3
#define font_height 5
#define font_size (font_width * font_height)

// clang-format off

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

extern const uint8_t * font_list[];

extern const int num_fonts;

static inline void font_draw(const uint8_t font[], color_t color, int startpos) {
    // insert the font_width*font_height font to the led_array
    // which has horizontalButtons*verticalButtons leds
    if (startpos < 0 || startpos >= NUM_LEDS ||
        startpos % horizontalButtons + font_width > horizontalButtons ||
        startpos / horizontalButtons + font_height > verticalButtons) {
        return;
    }
    for (int i = 0; i < font_size; i++) {
        if (font[i]) {
            int x = (font_size - i - 1) % font_width;
            int y = (font_size - i - 1) / font_width;
            int pos = startpos + x + y * horizontalButtons;
            set_color(pos, color);
        }
    }
}
