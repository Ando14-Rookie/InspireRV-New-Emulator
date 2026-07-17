#include "funconfig.h"
// #include "..\emulator\adriel's_2026_work\system_window.h"
#include <stdio.h>
#include "ws2812b_simple.h" // The compiler skips the file if it has already been read

// Need to compile and link C file, not header file
//How to run file: gcc switch_page.c "../emulator/adriel's_2026_work/system_window.c" -o switch_page.exe -lpthread

// Add function prototype
// void pthread_init(void);

// Start custom emulator terminal for testing
int main() {
    //Instansiate thread for listening to the 9 buttons
    // pthread_init();

    // // Discard any residual keystrokes from launching the program
    // Delay_Ms(300);
    // // Lock the thread from receiving any input
    // pthread_mutex_lock(&keyMutex);

    // // Clear whatever got captured during launch
    // pressedKeyCount = 0;      
    // currentKey = 0;
    // // Allow thread to take input
    // pthread_mutex_unlock(&keyMutex);

    // Ensure that the button state doesn't receive input
    checkNineButton();
    updateNineButton();
    
    // Initialize the foreground and background color
    initScaledForeBackColors();

    printf("Emulator started\n");
    // forces the C library to immediately write any data stored in the stdout (standard output) stream buffer to the actual console
    fflush(stdout);

    //Make the 8x8 LED matrix
    makeEmulatorScreen(led_array);
}