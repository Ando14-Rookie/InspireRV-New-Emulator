#include "extra_function.h"

#include <stdio.h>

// How to debug using int main(): gcc extra_function.c -I"WHERE_TO_OPEN_FILE" -o
// extra_function.exe

// Default is 0, should be 9 so that it can trigger True result
int currentKey = 0;

// Will be used in this file only
PageState prevPageState = PAINTING_SPACE;

/*
    Key just pressed → cur=true, prev=false → move once ✓
    Key still held → cur=true, prev=true → skip ✓
    Key released → cur=false → skip ✓
*/

void handleButtonFunction(void) {
    // Check for input from 1 to 9
    checkNineButton();

    // Only do anything if at least one button was just pressed
    bool anyPressed =
        BTN_JUST_PRESSED(BTN_1) || BTN_JUST_PRESSED(BTN_2) || BTN_JUST_PRESSED(BTN_3) ||
        BTN_JUST_PRESSED(BTN_4) || BTN_JUST_PRESSED(BTN_5) || BTN_JUST_PRESSED(BTN_6) ||
        BTN_JUST_PRESSED(BTN_7) || BTN_JUST_PRESSED(BTN_8) || BTN_JUST_PRESSED(BTN_9);

    if (!anyPressed) {
        // Update previous state and exit early — nothing to do
        updateNineButton();
        // printf("anypressed\n");
        return;
    }

    // Only printed when a button is actually pressed
    printf("Current page: %d\n", currentPage);

    // Check which page it is as it will have different functions
    // For Painting Space

    if (currentPage == PAINTING_SPACE) {
        if (BTN_JUST_PRESSED(BTN_1)) {
            printf("You selected 'LOAD'.\n");
            // Remember previous page: PAINTING_SPACE
            prevPageState = currentPage;
            // Change pageState to PAINTING_LOAD_SLOT
            currentPage = PAINTING_LOAD_SLOT;
            // Will print the page ID: 7
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_2)) {
            printf("You selected 'BRIGTHNESS CONTROL'.\n");
            printf("Now page state is %d\n", currentPage);
            // Remember previous page: CODING_SPACE
            prevPageState = currentPage;
            // Change pageState to BRIGHTNESS_CONTROL
            currentPage = BRIGHTNESS_CONTROL;
            // Will print the page ID: 13
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_3)) {
            printf("You selected 'SAVE 9 TO RESET AFTER SAVED'.\n");
            printf("Now page state is %d\n", currentPage);
            // Remember previous page: PAINTING_SPACE
            prevPageState = currentPage;
            // Change pageState to PAINTING_SAVE_SLOT
            currentPage = PAINTING_SAVE_SLOT;
            // Will print the page ID: 5
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_4)) {
            printf("You selected 'COLOR FOR FOREGROUND'.\n");
            pickingForeground = true;

            // Remember previous page: PAINTING_SPACE
            prevPageState = currentPage;
            // Change pageState to COLOR_PICK_FOREGROUND
            currentPage = COLOR_PICK_FOREGROUND;
            // Will print the page ID: 2
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_5)) {
            printf("You selected 'BINARY GAME'.\n");
            // Remember previous page: PAINTING_SPACE
            prevPageState = currentPage;
            // Change pageState to COLOR_PICK_FOREGROUND
            currentPage = BINARY_GAME;
            // Will print the page ID: 14
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_6)) {
            printf("You selected 'COLOR FOR BACKGROUND'.\n");
            pickingForeground = false;

            // Remember previous page: PAINTING_SPACE
            prevPageState = currentPage;
            // Change pageState to COLOR_PICK_BACKGROUND
            currentPage = COLOR_PICK_BACKGROUND;
            // Will print the page ID: 3
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_7)) {
            printf("You selected 'TO CODING SPACE'.\n");
            // Change pageState to CODING_SPACE
            currentPage = CODING_SPACE;
            // Start the coding space logic
            // initCodingGrid();
            // Clear current screen
            clear();
        }
        if (BTN_JUST_PRESSED(BTN_8)) {
            printf("You selected 'BUCKET FILL'.\n");
            printf("Now page state is %d\n", currentPage);
            // Remember previous page: PAINTING_SPACE
            prevPageState = currentPage;
            // Change pageState to COLOR_PICK_BACKGROUND
            currentPage = COLOR_PICK_BUCKET;
            // Will print the page ID: 4
            printf("Now page state is %d\n", currentPage);
        }
        if (BTN_JUST_PRESSED(BTN_9)) {
            printf("You selected 'CLEAR SCREEN'.\n");
            // Delete all color data & create new emulator screen again
            resetEmulatorScreen();
        }
    }
    // For Coding Space
    if (currentPage == CODING_SPACE) {
        if (simState != SIM_RUNNING) {
            if (BTN_JUST_PRESSED(BTN_1)) {
                printf("You selected 'LOAD'.\n");
                // Remember previous page: PAINTING_SPACE
                prevPageState = currentPage;
                // Change pageState to CODING_LOAD_SLOT
                currentPage = CODING_LOAD_SLOT;
                // Will print the page ID: 11
                printf("Now page state is %d\n", currentPage);
            }
            else if (BTN_JUST_PRESSED(BTN_2)) {
                printf("You selected 'BRIGTHNESS CONTROL'.\n");
                printf("Now page state is %d\n", currentPage);
                // Remember previous page: CODING_SPACE
                prevPageState = currentPage;
                // Change pageState to BRIGHTNESS_CONTROL
                currentPage = BRIGHTNESS_CONTROL;
                // Will print the page ID: 13
                printf("Now page state is %d\n", currentPage);
            }
            else if (BTN_JUST_PRESSED(BTN_3)) {
                printf("You selected 'SAVE 9 TO RESET AFTER SAVED'.\n");
                printf("Now page state is %d\n", currentPage);
                // Remember previous page: PAINTING_SPACE
                prevPageState = currentPage;
                // Change pageState to CODING_SAVE_SLOT
                currentPage = CODING_SAVE_SLOT;
                // Will print the page ID: 9
                printf("Now page state is %d\n", currentPage);
            }
            else if (BTN_JUST_PRESSED(BTN_5)) {
                printf("You selected 'RESULT'.\n");
                resultSimulation();
            }
            else if (BTN_JUST_PRESSED(BTN_6)) {
                printf("You selected 'RUN SIMULATION'.\n");
                // Allow momentarily for the function to be in SIM_RUNNING state
                startStepSimulation(speedVar);
            }
            else if (BTN_JUST_PRESSED(BTN_7)) {
                printf("You selected 'CLEAR'.\n");
            }
            else if (BTN_JUST_PRESSED(BTN_8)) {
                printf("You selected 'CLEAR CURRENT PAGE'.\n");
                resetCanvaScreen();
            }
            else if (BTN_JUST_PRESSED(BTN_9)) {
                printf("You selected 'TO PAINTING SPACE'.\n");
                // Change pageState to PAINTING_SPACE
                currentPage = PAINTING_SPACE;
            }
        }
        // Button 4 only work when simulation is running
        else if (simState == SIM_RUNNING && BTN_JUST_PRESSED(BTN_4)) {
            printf("You selected 'RETURN TO PROGRAMMING SPACE'.\n");
            stopStepSimulation();
        }
        else {
            printf("Key press is not allowed during simulation, press '4' to stop "
                   "simulation.\n");
        }
    }
    // Occur when user want to choose foreground or background color
    if ((currentPage == COLOR_PICK_BACKGROUND) ||
        (currentPage == COLOR_PICK_FOREGROUND)) {
        chooseColorForeBack();
    }

    if ((currentPage == COLOR_PICK_BUCKET)) {
        fullBucketFillLogic();
    }

    if (currentPage == PAINTING_SAVE_SLOT || currentPage == PAINTING_LOAD_SLOT) {
        saveOrLoadPaintToSlot();
    }

    if (currentPage == CODING_SAVE_SLOT || currentPage == CODING_LOAD_SLOT) {
        saveOrLoadCodeToSlot();
    }

    if (currentPage == BRIGHTNESS_CONTROL) {
        renderBrightnessSelectScreen();
    }

    if(currentPage == BINARY_GAME){
        initBinaryGame();
    }

    // Update the previous data to check if button actually being press, just press or
    // release
    updateNineButton();
}

bool isRunning(void) {
    // Returns false when 9 is pressed → exits the while loop
    return !(currentKey == _9_Key);
}
