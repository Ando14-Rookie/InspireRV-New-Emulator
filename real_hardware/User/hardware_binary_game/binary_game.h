#ifndef _BINARY_GAME_H
#define _BINARY_GAME_H

// 1. Declare hardware configurations required by driver.h
#ifndef WS2812BSIMPLE_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#endif

#ifndef INTERNAL_INSPIRE_MATRIX
#define INTERNAL_INSPIRE_MATRIX 1
#endif

// 2. Safely include your hardware peripheral library 

#ifdef abs 
#undef abs
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../data/colors.h"
#include "../ch32v003fun/driver.h"
#include "../ch32v003fun/ws2812b_simple.h"

#define noButtonPressed -1
#define LED_PINS GPIOA, 2
#define defaultLogoColor onColorBlue // Blue

/** 
 * @brief  Show and handle the Binary Game 
 * @param brightnessDivisor Brightness level to be used in each LED
 **/ 
extern void renderBinaryGameHW(uint8_t brightnessDivisor);

#endif