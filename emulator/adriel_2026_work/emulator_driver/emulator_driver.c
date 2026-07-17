#include "emulator_driver.h"
#include "../emulator/ws2812b_simple.h"

// Default page is PAINTING_SPACE
PageState currentPage = PAINTING_SPACE;

void SystemInitEmulator(void) {
    #if defined(_WIN32) || defined(_WIN64)
        // Set the console to UTF-8 mode
        SetConsoleOutputCP(65001);
        // Get the current console mode
        DWORD consoleMode;
        GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &consoleMode);
        // Enable virtual terminal processing
        consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), consoleMode);
    #elif defined(__APPLE__)
        // macOS supports UTF-8 and ANSI escape sequences natively!
        // No special console initialization is required.
    #endif
}

void resetEmulatorScreen(void) {
    currentposition = 0;
    buttonPressed = 0;

    for (int i = 0; i < NUM_LEDS; i++) {
        // reset Color
        savedColor[i] = offColor;
        // Set the toggleState to off
        ledCondition[i] = 0;
    }

    fill_color(offColor);
    clearScreen();
}

/**
 * @brief Get the key that is being pressed
 * 
 * Argument type KeyCode_t is either WORD or CGKeyCode
**/
bool is_key_pressed(KeyCode_t capitalkey) {
    #if defined(_WIN32) || defined(_WIN64)
        SHORT result =
            GetAsyncKeyState((int)capitalkey); // windows.h requires capital letters
        return (result & 0x8000) != 0;

    #elif defined (__APPLE__)
        bool pressed = false;

        // Lock the mutex so the event thread doesn't modify the array while we read it
        pthread_mutex_lock(&keyMutex);
        
        for (int i = 0; i < pressedKeyCount; i++) {
            if (pressedKeys[i] == capitalkey) {
                pressed = true;
                break; // Key found!
            }
        }
        
        pthread_mutex_unlock(&keyMutex);
        return pressed;
    #endif
}

// uint16_t ADC_read(void) {
//     // If pressed A, B, C, D, wait for second input 0-9 and A-F
//     // return the value of the button
//     // e.g. AF indicates A=0 + F=15 = 15
//     // Use non blocking is_key_pressed
//     for (char i = 'A'; i <= 'D'; i++) {
//         if (is_key_pressed(i)) {
//             while (is_key_pressed(i))
//                 ;
//             printf("You pressed %c, Press 0-9 or A-F\n", i);
//             while (true) {
//                 for (char j = '0'; j <= '9'; j++) {
//                     if (is_key_pressed(j)) {
//                         printf("Pressed %c\n", j);
//                         const int BUTTON_INDEX = (i - 'A') * 16 + (j - '0');
//                         if (BUTTON_INDEX > NUM_BUTTONS - 1) {
//                             return 0;
//                         }
//                         return buttons[BUTTON_INDEX];
//                     }
//                 }
//                 for (char j = 'A'; j <= 'F'; j++) {
//                     if (is_key_pressed(j)) {
//                         printf("Pressed %c\n", j);
//                         const int BUTTON_INDEX = (i - 'A') * 16 + (j - 'A' + 10);
//                         if (BUTTON_INDEX > NUM_BUTTONS - 1) {
//                             return 0;
//                         }
//                         return buttons[BUTTON_INDEX];
//                     }
//                 }
//             }
//         }
//     }
//     return 0;
// }

// uint16_t ADC_read(void) {
//     // If pressed A, B, C, D, wait for second input 0-9 and A-F
//     // return the value of the button
//     // e.g. AF indicates A=0 + F=15 = 15
//     // Use non blocking is_key_pressed
//     for (int i = 0; i < 4; i++) {
//         if (is_key_pressed(ABCD[i])) {
//             while (is_key_pressed(ABCD[i]))
//                 ;
//             printf("Pressed %c, Press 0-9 or A-F\n", ABCD[i]);
//             while (true) {
//                 for (int j = 0; j < 16; j++) {
//                     if (is_key_pressed(_0123456789ABCDEF[j])) {
//                         printf("Pressed %d\n", j);
//                         const int BUTTON_INDEX = i * 16 + j;
//                         if (BUTTON_INDEX > NUM_BUTTONS - 1) {
//                             return 0;
//                         }
//                         return buttons[BUTTON_INDEX];
//                     }
//                 }
//             }
//         }
//     }
//     return 0;
// }

// int8_t matrix_pressed(uint16_t (*matrix)(void)) {
//     uint16_t adc_value = matrix();
//     for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
//         if (buttons[i] == adc_value) {
//             return i;
//         }
//     }
//     return no_button_pressed;
// }