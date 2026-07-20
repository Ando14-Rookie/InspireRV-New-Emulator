#include "colors.h"

/// @brief buffer
color_t led_array[NUM_LEDS] = {0};

// clang-format off
const color_t colors[NUM_LEDS] = {
    {15, 3, 53}, {29, 3, 44}, {50, 0, 7}, {34, 57, 0}, {19, 54, 0}, {7, 57, 0}, {6, 34, 55}, {0, 0, 0},
    {42, 1, 108}, {61, 1, 95}, {80, 3, 14}, {73, 103, 3}, {36, 107, 4}, {18, 107, 14}, {16, 62, 106}, {30, 29, 35},
    {55, 9, 166}, {85, 14, 147}, {90, 5, 32}, {113, 147, 5}, {60, 161, 18}, {39, 149, 22}, {32, 104, 168}, {55, 54, 60},
    {82, 18, 214}, {122, 5, 199}, {113, 0, 57}, {148, 200, 2}, {86, 210, 24}, {53, 216, 48}, {51, 135, 207}, {83, 83, 95},
    {107, 25, 255}, {161, 1, 237}, {148, 2, 74}, {183, 249, 1}, {109, 253, 39}, {74, 255, 68}, {76, 169, 252}, {121, 115, 135},
    {121, 62, 251}, {175, 42, 244}, {183, 33, 107}, {197, 249, 44}, {128, 255, 65}, {100, 253, 86}, {101, 176, 254}, {153, 146, 163},
    {159, 104, 250}, {188, 92, 246}, {197, 84, 138}, {212, 246, 79}, {157, 254, 112}, {140, 252, 129}, {135, 199, 255}, {184, 183, 189},
    {191, 167, 255}, {212, 151, 255}, {254, 144, 138}, {222, 254, 147}, {195, 255, 158}, {177, 255, 177}, {178, 214, 252}, {211, 212, 217}

};

// clang-format on

const uint16_t num_colors = NUM_LEDS;

color_t color_divide(color_t color, uint8_t divider) {
    return (color_t){color.r / divider, color.g / divider, color.b / divider};
}
uint8_t ledDivisor = 10;

void set_color(uint8_t led, color_t color, uint8_t ledDivisor) {
    //uint8_t divider = 20;
    led_array[led].r = smaller(color.r, color.r / ledDivisor);
    led_array[led].g = smaller(color.g, color.g / ledDivisor);
    led_array[led].b = smaller(color.b, color.b / ledDivisor);
}

void set_color_no_div(uint8_t led, color_t color) {
    //uint8_t divider = 20;
    led_array[led].r = color.r;
    led_array[led].g = color.g;
    led_array[led].b = color.b;
}

/** 
 * @brief Fill the whole LED matrix with desired  color and brightness level
 * @param color The color for each LED
 * 
 **/
void fill_color(color_t color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        set_color(i, color, ledDivisor);
    }
}

void clear(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        set_color(i, (color_t){0, 0, 0},ledDivisor);
    }
}

/// @brief Handle the initial `RV logo screen` in the beginning of InspireRV 
void fill_logo(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        switch(i){
            case 63:
            case 62:
            case 61:
            case 60:
            case 55:
            case 51:
            case 47:
            case 46:
            case 45:
            case 44:
            case 43:
            case 39:
            case 37:
            case 31:
            case 28:
            case 23:
            case 19:
                set_color(i, (color_t){0, 100, 0},ledDivisor);
                break;
            case 2:
            case 11:
            case 9:
            case 20:
            case 16:
            case 24:
            case 36:
            case 32:
                set_color(i, (color_t){0, 0, 100},ledDivisor);
            break;
            default:
                set_color(i, (color_t){0, 0, 0},ledDivisor);
            break;
        }
    }
}