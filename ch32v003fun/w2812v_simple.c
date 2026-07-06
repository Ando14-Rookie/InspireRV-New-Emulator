#include "ws2812b_simple.h"

#if defined(EMULATOR_BUILD) || defined(_WIN32)

// ── Emulator stub ─────────────────────────────────────────────
void WS2812BSimpleSend(GPIO_TypeDef *port, int pin, uint8_t *data, int len_in_bytes)
{
    (void)port; (void)pin; (void)data; (void)len_in_bytes;
    // No hardware on PC — do nothing
}

#else

// ── Real CH32V003 hardware ────────────────────────────────────
#if FUNCONF_SYSTICK_USE_HCLK != 1
#error WS2812B Driver requires FUNCONF_SYSTICK_USE_HCLK = 1
#endif

void WS2812BSimpleSend(GPIO_TypeDef *port, int pin, uint8_t *data, int len_in_bytes)
{
    int port_id = (((intptr_t)port - (intptr_t)GPIOA) >> 10);
    RCC->APB2PCENR |= (RCC_APB2Periph_GPIOA << port_id);

    int poffset = (pin * 4);
    port->CFGLR = (port->CFGLR & (~(0xf << poffset))) |
                  ((GPIO_Speed_2MHz | GPIO_CNF_OUT_PP) << poffset);

    int maskon  = 1 << pin;
    int maskoff = 1 << (16 + pin);

    port->BSHR = maskoff;

    uint8_t *end = data + len_in_bytes;
    while (data != end)
    {
        uint8_t byte = *data;
        for (int i = 0; i < 8; i++)
        {
            if (byte & 0x80)
            {
                port->BSHR = maskon;
                DelaySysTick(25);
                port->BSHR = maskoff;
                DelaySysTick(1);
            }
            else
            {
#ifndef WS2812BSIMPLE_NO_IRQ_TWEAKING
                __disable_irq();
#endif
                port->BSHR = maskon;
                asm volatile("nop\nnop\nnop\nnop");
                port->BSHR = maskoff;
#ifndef WS2812BSIMPLE_NO_IRQ_TWEAKING
                __enable_irq();
#endif
                DelaySysTick(15);
            }
            byte <<= 1;
        }
        data++;
    }

    port->BSHR = maskoff;
}

#endif // EMULATOR_BUILD

// PREVIOUS CODE

// Only compile this file when targeting real CH32V003 hardware.
// For emulator builds, the stub in ws2812b_simple.h is used instead.

// #ifndef EMULATOR_BUILD

// #define WS2812BSIMPLE_IMPLEMENTATION
// #include "ws2812b_simple.h"

// #if FUNCONF_SYSTICK_USE_HCLK != 1
// #error WS2812B Driver requires FUNCONF_SYSTICK_USE_HCLK = 1 in funconfig.h
// #endif

// void WS2812BSimpleSend(GPIO_TypeDef *port, int pin, uint8_t *data, int len_in_bytes)
// {
//     int port_id = (((intptr_t)port - (intptr_t)GPIOA) >> 10);
//     RCC->APB2PCENR |= (RCC_APB2Periph_GPIOA << port_id);

//     int poffset = (pin * 4);
//     port->CFGLR = (port->CFGLR & (~(0xf << poffset))) |
//                   ((GPIO_Speed_2MHz | GPIO_CNF_OUT_PP) << poffset);

//     int maskon  = 1 << pin;
//     int maskoff = 1 << (16 + pin);

//     port->BSHR = maskoff;

//     uint8_t *end = data + len_in_bytes;
//     while (data != end)
//     {
//         uint8_t byte = *data;
//         for (int i = 0; i < 8; i++)
//         {
//             if (byte & 0x80)
//             {
//                 port->BSHR = maskon;
//                 DelaySysTick(25);
//                 port->BSHR = maskoff;
//                 DelaySysTick(1);
//             }
//             else
//             {
// #ifndef WS2812BSIMPLE_NO_IRQ_TWEAKING
//                 __disable_irq();
// #endif
//                 port->BSHR = maskon;
//                 asm volatile("nop\nnop\nnop\nnop");
//                 port->BSHR = maskoff;
// #ifndef WS2812BSIMPLE_NO_IRQ_TWEAKING
//                 __enable_irq();
// #endif
//                 DelaySysTick(15);
//             }
//             byte <<= 1;
//         }
//         data++;
//     }

//     port->BSHR = maskoff;
// }

// #endif // EMULATOR_BUILD