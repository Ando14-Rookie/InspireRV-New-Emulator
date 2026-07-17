#include "colors.h"

color_t onColorGreen = {100, 255, 100}; // Green
color_t onColorYellow = {255, 255, 100}; // Yellow
color_t onColorPurple = {165, 100, 255}; // Purple
color_t onColorBlue = {100, 100, 255};  // Blue

const color_t fixedColorGreen = {100, 255, 100}; // Green
const color_t offColor = {0, 0, 0};           // Black
const color_t pointerColor = {255, 188, 100}; // Orange
const color_t solidColorRed = {255, 0, 0}; // Solid Red
const color_t lightColorRed = {255, 200, 200};  // Light Red

/// Buffer color data for 8x8 LED Matrix
color_t led_array[NUM_BUTTONS] = {0};

/// the real saved color for 8x8 LED Matrix
color_t savedColor[NUM_BUTTONS] = {0};

//clang format off
const color_t colors[NUM_BUTTONS] = {
    {15, 3, 53}, {29, 3, 44}, {28, 0, 14}, {34, 57, 0}, {19, 54, 0}, {7, 57, 0}, {6, 34, 55}, {0, 0, 0},
    {42, 1, 108}, {61, 1, 95}, {55, 3, 32}, {73, 103, 3}, {36, 107, 4}, {18, 107, 14}, {16, 62, 106}, {30, 29, 35},
    {55, 9, 166}, {85, 14, 147}, {81, 5, 42}, {113, 147, 5}, {60, 161, 18}, {39, 149, 22}, {32, 104, 168}, {55, 54, 60},
    {82, 18, 214}, {122, 5, 199}, {110, 0, 57}, {148, 200, 2}, {86, 210, 24}, {53, 216, 48}, {51, 135, 207}, {83, 83, 95},
    {107, 25, 255}, {161, 1, 237}, {131, 2, 74}, {183, 249, 1}, {109, 253, 39}, {74, 255, 68}, {76, 169, 252}, {121, 115, 135},
    {121, 62, 251}, {175, 42, 244}, {160, 33, 107}, {197, 249, 44}, {128, 255, 65}, {100, 253, 86}, {101, 176, 254}, {153, 146, 163},
    {159, 104, 250}, {188, 92, 246}, {181, 84, 138}, {212, 246, 79}, {157, 254, 112}, {140, 252, 129}, {135, 199, 255}, {184, 183, 189},
    {191, 167, 255}, {212, 151, 255}, {204, 144, 176}, {222, 254, 147}, {195, 255, 158}, {177, 255, 177}, {178, 214, 252}, {211, 212, 217}
};

color_t colorfulMap[NUM_BUTTONS] = {};

const uint16_t num_colors = NUM_LEDS;

void setColorLEDScaled(uint8_t index, color_t color, uint8_t divisor) {
    // printf("Brightness by global var is %d\n", divisor);
    color_t scaled = {.r = 0, .g = 0, .b=0};
    scaled.r = color.r / divisor;
    scaled.g = color.g / divisor;
    scaled.b = color.b / divisor;
    // printf("Before led_array: %d, with scaled: %d \n", led_array[index], scaled);
    led_array[index] = scaled;
    // printf("After led_array: %d, with scaled: %d \n", led_array[index], scaled);
}

// void changeColorScaled(color_t *storageArray, uint8_t index, color_t newColor, uint8_t divisor){
//     storageArray[index].r = newColor.r / divisor;
//     storageArray[index].g = newColor.g/ divisor;
//     storageArray[index].b = newColor.b/ divisor;
// }

color_t initColorScaled(color_t color, uint8_t divisor){
    color_t scaled;
    scaled.r = color.r / divisor;
    scaled.g = color.g / divisor;
    scaled.b = color.b / divisor;

    return scaled;
}

// To scale brightness down without changing the basic color
color_t color_divide(color_t color, uint8_t divider) {
    return (color_t){color.r / divider, color.g / divider, color.b / divider};
}

void set_color(uint8_t led, color_t color) {
    uint8_t divider = 1; //TODO: Meant for brightness control
    //led_array has data type of struct "color_t"
    led_array[led].r = smaller(color.r, color.r / divider);
    led_array[led].g = smaller(color.g, color.g / divider);
    led_array[led].b = smaller(color.b, color.b / divider);
}

//Fill each LED with color
void fill_color(color_t color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        set_color(i, color);
    }
}

// Clear screen
void clear(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        set_color(i, (color_t){0, 0, 0});
    }
}

