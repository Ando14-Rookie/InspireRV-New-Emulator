// #pragma once

#ifndef EMULATOR_DRIVER_H
#define EMULATOR_DRIVER_H

// PageState defined at top level so it's always available
#ifndef PAGE_STATE_H
#define PAGE_STATE_H
typedef enum {
    PAINTING_SPACE = 0,
    CODING_SPACE,
    COLOR_PICK_FOREGROUND,
    COLOR_PICK_BACKGROUND,
    COLOR_PICK_BUCKET,
    PAINTING_SAVE_SLOT,
    PAINTING_SAVE_CONFIRM,
    PAINTING_LOAD_SLOT,
    PAINTING_LOAD_CONFIRM,
    CODING_SAVE_SLOT,
    CODING_SAVE_CONFIRM,
} PageState;

extern PageState currentPage;
#endif

// #include "..\new_emulator_system\funconfig.h"
// #include "..\ch32v003fun\ws2812b_simple.h"
#include "../../emulator/ws2812b_simple.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../../emulator-screen/led_matrix_screen.h"

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX 1          // Prevent Windows.h from defining min and max macros
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "../system_window.h"


void SystemInitEmulator(void);
void resetEmulatorScreen(void);

#define SystemInit() SystemInitEmulator()
#define Delay_Ms(milliseconds) Sleep(milliseconds)
#define Delay_Us(microseconds) Sleep((microseconds) / 1000)

// Replaces everry instance of JOY_###_pressed() with the corresponding key press
//  Code Space & Paint Space: load
#define JOY_first_pressed() is_key_pressed('1')
// Code Space & Paint Space: brightness control
#define JOY_second_pressed() !is_key_pressed('2')
// Code Space & Paint Space: press to save or after press 3, u press 9 to reset save
#define JOY_third_pressed() is_key_pressed('3')
// Code Space: return to proramming space ;Paint Space: color for foreground
#define JOY_fourth_pressed() is_key_pressed('4')
// Code Space: immediate code result ;Paint Space: None
#define JOY_fifth_pressed() is_key_pressed('5')
// Code Space: run simulation ;Paint Space: color for background
#define JOY_sixth_pressed() is_key_pressed('6')
// Code Space: clear ;Paint Space: to coding space
#define JOY_seventh_pressed() is_key_pressed('7')
// Code Space: clear current page ;Paint Space: bucket fill
#define JOY_eigth_pressed() is_key_pressed('8')
// Code Space: go to painting space ;Paint Space: clear screen
#define JOY_ninth_pressed() is_key_pressed('9')

// Replaces everry instance of JOY_###_pressed() with the corresponding key press
#define JOY_enter_pressed() is_key_pressed(Enter_Key)
#define JOY_up_pressed() is_key_pressed('I')
#define JOY_down_pressed() is_key_pressed('K')
#define JOY_left_pressed() is_key_pressed('J')
#define JOY_right_pressed() is_key_pressed('L')

/**
 * @brief Get the key that is being pressed
 * 
**/
bool is_key_pressed(char capitalkey);

// static: make function only visible in this file; 
// inline: suggest compiler to replace function call with actual code to reduce overhead
/**
 * @brief Check condition which key to check (TRUE || FALSE) 
 * 
 * `result` stores the raw 16-bit value returned by GetAsyncKeyState. 
 *  `(result & 0x8000) != 0` takes the MSB and compare with 0. If the top bit is set, it returns TRUE
 * @param capitalkey the input key you want to test, and it gets cast to int
 * 
**/
SHORT getKeyPressed(void);

/**
 * @brief Watches the keyboard, waits for a 2-character input like `A0`, `AF`, `C7`, or `D3`
 * 
 * It converts the input into a button index, and then returns the corresponding value from the `buttons[]` array, 
 * mapping key inputs to stored button values is a common way to emulate button/ADC behavior in test code.
 * 
**/
uint16_t ADC_read(void);

#elif defined(__APPLE__)
#include "system_mac.h"
#include <unistd.h>

#define SystemInit() pthread_init()
#define Delay_Ms(milliseconds) usleep((milliseconds) * 1000)
#define Delay_Us(microseconds) usleep(microseconds)
#define JOY_up_pressed() is_key_pressed(I_Key)
#define JOY_down_pressed() is_key_pressed(K_Key)
#define JOY_left_pressed() is_key_pressed(J_Key)
#define JOY_right_pressed() is_key_pressed(L_Key)
#define JOY_enter_pressed() is_key_pressed(Enter_Key)

uint16_t ADC_read(void);

#endif // Check Window/MacOS

#define JOY_pad_pressed()                                                                \
    (JOY_up_pressed() || JOY_down_pressed() || JOY_left_pressed() || JOY_right_pressed())
#define JOY_pad_released()                                                               \
    (!JOY_up_pressed() && !JOY_down_pressed() && !JOY_left_pressed() &&                  \
        !JOY_right_pressed())
#define JOY_all_released() (JOY_act_released() && !JOY_pad_released())

// void ADC_init(void) {
//     // Do nothing
// }

#define no_button_pressed -1

/** 
 * @brief  Identifies which one of the matrix button is pressed based on the returned ADC/button value.”
 * 
 * `buttons[]` stores the expected value for each button.
 * 
 * @param matrix call `matrix()` to get a value
 * 
**/
int8_t matrix_pressed(uint16_t (*matrix)(void));

static inline uint16_t ADC_read_pad(void) { return ADC_read(); }

static inline uint16_t ADC_read_smallboard(void) { return ADC_read(); }

// 4 same functions has been moved to this path file below
// #include "../common_function/same_driver.h"

#define matrix_pressed_two() matrix_pressed(ADC_read)

#endif //Header guard ending for emulator_driver.h
