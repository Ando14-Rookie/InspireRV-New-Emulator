// Better practice: guard is unique and clearly tied to that header
#ifndef LED_MATRIX_SCREEN_H
#define LED_MATRIX_SCREEN_H

#define CH32V003 1
#define FUNCONF_USE_DEBUGPRINTF 0
#define FUNCONF_USE_UARTPRINTF 1
#define FUNCONF_UART_PRINTF_BAUD 115200
#define horizontalButtons 8
#define verticalButtons 8
#define CH32V003J4M6_USE_PD6_AS_UART_TX
#define FUNCONF_SYSTICK_USE_HCLK 1
#define WS2812BSIMPLE_IMPLEMENTATION

#include <stdio.h>
#include <stdint.h>

#include "../data/colors.h"
#include "../data/buttons.h"
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
// #include "../emulator/ws2812b_simple.h"
#include "../painting_space/painting_space.h"
#include "../emulator/adriel_2026_work/extra_function.h"
// #include "../ch32v003fun/driver.h"

#define LED_PINS GPIOC, 2
# define clearScreen() clear()

extern uint8_t currentposition;
extern uint8_t toggle[NUM_LEDS];
//Prevent emulator from running infinitely
extern int buttonPressed;

/** 
 * @brief Create the emulator screen in the terminal
 * @param arr pass the memory address of the array itself for memory efficiency
**/
void makeEmulatorScreen(color_t *arr);

#endif
