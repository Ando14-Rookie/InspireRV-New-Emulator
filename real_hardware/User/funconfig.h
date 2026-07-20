#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define CH32V003 1
#define FUNCONF_USE_DEBUGPRINTF 0
#define FUNCONF_USE_UARTPRINTF 1
#define FUNCONF_UART_PRINTF_BAUD 115200
#define horizontalButtons 8
#define verticalButtons 8
#define NUM_LEDS (horizontalButtons * verticalButtons)

#define FUNCONF_SYSTICK_USE_HCLK 1
#define INTERNAL_INSPIRE_MATRIX 1

/** 
 * @brief Re-renders every LED on the physical strip based on the current contents 
 * of your `canvas[] array`, and then physically sends out by WS2812B protocol
 **/
extern void flushCanvas(void);

/** 
 * @brief Create color pallete that user can choose from the 8x8 LED Matrix.
 * It is used to `select new foreground and background color`
 * 
 **/
extern void displayColorPalette(void);

#endif
