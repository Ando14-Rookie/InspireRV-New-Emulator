#include "funconfig.h"
// #include "..\emulator\adriel's_2026_work\system_window.h"
#include <stdio.h>

// Need to compile and link C file, not header file
//How to run file: gcc switch_page.c "../emulator/adriel's_2026_work/system_window.c" -o switch_page.exe -lpthread

// Start custom emulator terminal for testing
int main() {
    //Instansiate thread
    pthread_init();
    //Instansiate pageState, default must be painting space
    // strcpy(pageState, "PAINTING_SPACE");

    printf("Emulator started\n");
    // forces the C library to immediately write any data stored in the stdout (standard output) stream buffer to the actual console
    fflush(stdout);

    while (1) {
        // Prevent high usage of CPU by sleeping for a short duration before check again
        Sleep(100);
    }
}