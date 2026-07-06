#ifndef WS2812B_SIMPLE_H
#define WS2812B_SIMPLE_H

#include <stdint.h>

#if defined(EMULATOR_BUILD) || defined(_WIN32) || defined(__APPLE__)
    #include "../emulator/ch32v003fun.h"
#else
    #include "../ch32v003fun/ch32v003fun.h"
#endif

// Single declaration — works for both emulator and hardware
void WS2812BSimpleSend(GPIO_TypeDef *port, int pin, uint8_t *data, int len_in_bytes);

#endif // WS2812B_SIMPLE_H