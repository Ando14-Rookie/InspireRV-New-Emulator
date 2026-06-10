#pragma once // ensures a header file is included only once during a single compilation
#ifdef _WIN32
#include "ws2812b_simple.h"
#include "buttons.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#define NOMINMAX 1          // Prevent Windows.h from defining min and max macros
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define Delay_Ms(milliseconds) Sleep(milliseconds)
#define Delay_Us(microseconds) Sleep((microseconds) / 1000)
// Replaces everry instance of JOY_###_pressed() with the corresponding key press
//  Code Space & Paint Space: load
// #define JOY_act_pressed()                                                                \
//     (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_ACT)
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

const int horizontalButtons = 8;
const int verticalButtons = 8;


// Prepares the Windows console so terminal output behaves better
// Configure Window CMD (Console) for detecting live keyboard clicks for a program or game
void SystemInit(void) {
    // Set the console to UTF-8 mode
    SetConsoleOutputCP(65001);
    // Get the current console mode
    DWORD consoleMode;
    // Get the console window that represents the program’s output screen
    // Read the current settings of that console output.
    // Finally, this will contain the current console configuration
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &consoleMode);
    // Enable virtual terminal processing: translate them into actual terminal actions (e.g., moving cursor, changing colors, etc.)
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    // Save the updated settings back to the console.
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), consoleMode);
}

// static: make function only visible in this file; 
// inline: suggest compiler to replace function call with actual code to reduce overhead
static inline bool is_key_pressed(char capitalkey) {
    // GetAsyncKeyState: current state of that key
    SHORT result =
        GetAsyncKeyState((int)capitalkey); // windows.h requires capital letters

    // Check the pressed-down flag; if it is on, return true.
    return (result & 0x8000) != 0;
}

//TODO: TO BE CONTINUED

uint16_t ADC_read(void) {
    // If pressed A, B, C, D, wait for second input 0-9 and A-F
    // return the value of the button
    // e.g. AF indicates A=0 + F=15 = 15
    // Use non blocking is_key_pressed
    for (char i = 'A'; i <= 'D'; i++) {
        if (is_key_pressed(i)) {
            while (is_key_pressed(i))
                ;
            printf("Pressed %c, Press 0-9 or A-F\n", i);
            while (true) {
                for (char j = '0'; j <= '9'; j++) {
                    if (is_key_pressed(j)) {
                        printf("Pressed %c\n", j);
                        const int BUTTON_INDEX = (i - 'A') * 16 + (j - '0');
                        if (BUTTON_INDEX > NUM_BUTTONS - 1) {
                            return 0;
                        }
                        return buttons[BUTTON_INDEX];
                    }
                }
                for (char j = 'A'; j <= 'F'; j++) {
                    if (is_key_pressed(j)) {
                        printf("Pressed %c\n", j);
                        const int BUTTON_INDEX = (i - 'A') * 16 + (j - 'A' + 10);
                        if (BUTTON_INDEX > NUM_BUTTONS - 1) {
                            return 0;
                        }
                        return buttons[BUTTON_INDEX];
                    }
                }
            }
        }
    }
    return 0;
}

#elif defined(__APPLE__)
#include "system_mac.h"

#include <unistd.h>

#define SystemInit() pthread_init()
#define Delay_Ms(milliseconds) usleep((milliseconds) * 1000)
#define Delay_Us(microseconds) usleep(microseconds)
#define JOY_act_pressed() is_key_pressed(P_Key)
#define JOY_act_released() !is_key_pressed(P_Key)
#define JOY_up_pressed() is_key_pressed(I_Key)
#define JOY_down_pressed() is_key_pressed(K_Key)
#define JOY_left_pressed() is_key_pressed(J_Key)
#define JOY_right_pressed() is_key_pressed(L_Key)
#define JOY_X_pressed() is_key_pressed(U_Key)
#define JOY_Y_pressed() is_key_pressed(O_Key)

uint16_t ADC_read(void) {
    // If pressed A, B, C, D, wait for second input 0-9 and A-F
    // return the value of the button
    // e.g. AF indicates A=0 + F=15 = 15
    // Use non blocking is_key_pressed
    for (int i = 0; i < 4; i++) {
        if (is_key_pressed(ABCD[i])) {
            while (is_key_pressed(ABCD[i]))
                ;
            printf("Pressed %c, Press 0-9 or A-F\n", ABCD[i]);
            while (true) {
                for (int j = 0; j < 16; j++) {
                    if (is_key_pressed(_0123456789ABCDEF[j])) {
                        printf("Pressed %d\n", j);
                        const int BUTTON_INDEX = i * 16 + j;
                        if (BUTTON_INDEX > NUM_BUTTONS - 1) {
                            return 0;
                        }
                        return buttons[BUTTON_INDEX];
                    }
                }
            }
        }
    }
    return 0;
}

#endif

#define JOY_pad_pressed()                                                                \
    (JOY_up_pressed() || JOY_down_pressed() || JOY_left_pressed() || JOY_right_pressed())
#define JOY_pad_released()                                                               \
    (!JOY_up_pressed() && !JOY_down_pressed() && !JOY_left_pressed() &&                  \
        !JOY_right_pressed())
#define JOY_all_released() (JOY_act_released() && !JOY_pad_released())

void ADC_init(void) {
    // Do nothing
}

#define no_button_pressed -1
int8_t matrix_pressed(uint16_t (*matrix)(void)) {
    uint16_t adc_value = matrix();
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i] == adc_value) {
            return i;
        }
    }
    return no_button_pressed;
}

uint16_t ADC_read_pad(void) { return ADC_read(); }

uint16_t ADC_read_smallboard(void) { return ADC_read(); }

uint16_t rnval;
uint16_t JOY_random(void) {
    rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
    return rnval;
}

void JOY_setseed_default(void) { rnval = 0x1234; }

void JOY_setseed(uint16_t seed) { rnval = seed; }

#define matrix_pressed_two() matrix_pressed(ADC_read)
