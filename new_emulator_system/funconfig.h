// If macro/definition is not defined, define it. Otherwise, skip the content
#ifndef FUNCONFIG_H
    #define FUNCONFIG_H

    #define CH32V003 1
    #define FUNCONF_USE_DEBUGPRINTF 0
    #define FUNCONF_USE_UARTPRINTF 1
    #define FUNCONF_UART_PRINTF_BAUD 115200
    // extern const int NUM_LEDS;
    // extern const int horizontalButtons_1;
    // extern const int verticalButtons_1;

    
    // #define NUM_LEDS (horizontalButtons_1 * verticalButtons_1)

    #define CH32V003J4M6_USE_PD6_AS_UART_TX
    #define FUNCONF_SYSTICK_USE_HCLK 1

    #include "../emulator/adriel_2026_work/system_window_mac.h"
    #include <stdbool.h>
    #include <stdlib.h>
    #include "../data/buttons.h"
    #include "../emulator-screen/led_matrix_screen.h"

#endif
