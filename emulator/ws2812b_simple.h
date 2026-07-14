// #pragma once

#ifndef WS2812B_SIMPLE_H
#define WS2812B_SIMPLE_H
#include "../data/buttons.h"
#include "ch32v003fun.h"

#include <stdio.h>

#define BORDER_X 2

/**
 * @brief Emulator version of the real hardware LED driver
 *
 * @param port The GPIO port connected to your LED data line (e.g., GPIOA, GPIOC)
 * @param pin The specific pin number on that port (e.g., 4 for PC4)
 * @param data A pointer to an array of color data, typically organized in RGB format
 * @param len_in_bytes The total length of the data to send.
 *
 * For example, if you have 16 LEDs, the length should be 16 × 3 = 48 bytes
 **/
static inline void WS2812BSimpleSend(
    GPIO_TypeDef * port, int pin, uint8_t * data, int len_in_bytes) {
    (void)port;
    (void)pin;
    // printf("total len_in_bytes: %d", len_in_bytes);
    // checks whether the size of the input color data is correct.
    // printf("total len_in_bytes: %d", len_in_bytes);
    // checks whether the size of the input color data is correct.
    if (len_in_bytes != NUM_LEDS * 3) {
        printf("Error: len_in_bytes != NUM_LEDS * 3\n");
        while (1)
            ; // if the data size is wrong, the program stops here forever by infinite
              // loop.
        while (1)
            ; // if the data size is wrong, the program stops here forever by infinite
              // loop.
    }
    // Completely clear the console screen and move the cursor back to the top-left corner
    // Completely clear the console screen and move the cursor back to the top-left corner
    printf("\033[2J\033[H");

    // Printing "\x76543210"

    // Printing "\x76543210"
    printf("\\x");
    // Drawing the top x-axis numbers, count backwards
    // Drawing the top x-axis numbers, count backwards
    for (uint8_t i = HORIZONTAL_BUTTONS; i > 0; i--) {
        printf("%d", (i - 1) % 10); // prints the column numbers across the top
        printf("%d", (i - 1) % 10); // prints the column numbers across the top
    }

    // Printing "y----------"

    // Printing "y----------"
    printf(" \ny");
    for (uint8_t i = 0; i < HORIZONTAL_BUTTONS + BORDER_X; i++) {
        printf("-");
    }
    printf("\n");

    /*
        data[led * 3 + 1],   // this position holds .r in memory
        data[led * 3 + 0],   // this position holds .g in memory
        data[led * 3 + 2]);  // this position holds .b in memory
    */

    // Print the canvas
    for (uint8_t y = VERTICAL_BUTTONS; y > 0; y--) {
        printf("%d|", (y - 1) % 10);
        for (uint8_t x = HORIZONTAL_BUTTONS; x > 0; x--) {
            uint8_t led = (y - 1) * HORIZONTAL_BUTTONS + (x - 1);
            // print one █ character using this LED's RGB color, then reset terminal color
            printf("\x1b[38;2;%d;%d;%dm█\x1b[0m", data[led * 3 + 0], data[led * 3 + 1],
                data[led * 3 + 2]);
            // printf("\e[38;2;%d;%d;%dm\u2588\x1b[0m", data[led * 3], data[led * 3 + 1],
            //     data[led * 3 + 2]);
            // printf("\e[38;2;%d;%d;%dm\u2588\x1b[0m", data[led * 3], data[led * 3 + 1],
            //     data[led * 3 + 2]);
            // printf("r:%d g:%d b:%d ", data[led * 3], data[led * 3 + 1], data[led * 3 +
            // 2]);
        }
        printf("|\n");
    }
    // Printing the last part " ----------"
    // Printing the last part " ----------"
    printf(" ");
    for (uint8_t i = 0; i < HORIZONTAL_BUTTONS + BORDER_X; i++) {
        printf("-");
    }
    printf("\n");
}

#endif
