#include "./brightness_control.h"

uint8_t brightnessDivisor = 1;

void createBrightnessSelectionScreen(void) {
    fill_color(offColor);

    int row = 0; // bottom row, same convention as slot selection
    for (int level = 0; level < 8; level++) {
        int col = 7 - level; // level 0 (brightest) on the rightmost LED
        int idx = row * 8 + col;

        // divisor = level+1, so level 0 -> divisor 1 (brightest), level 7 -> divisor 8
        // (dimmest)
        setColorLEDScaled(idx, fixedColorGreen, col + 1);
    }

    // Draw pointer ON TOP visually (only effect led_array), doesn't touch
    // savedColor (real saved data)
    set_color(currentposition, pointerColor);
    printf("Pointer current position is %d\n ", currentposition);

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    printf("Brightness selection row displayed\n");
}

void renderBrightnessSelectScreen(void) {
    // if brightness level is selected, stop function
    bool divisorChosen = false;
    int buttonPressed = 0;

    // Reset prev states once before the loop
    updateMoveButton();

    // As soon as user enter this program, print brightness selection screen once
    createBrightnessSelectionScreen();
    
    while (!divisorChosen) {
        // Activate keyboard I, J, K, L press input
        checkMoveButton();

        // Phase 1: navigating and choose
        if (BTN_JUST_PRESSED(BTN_UP)) {
            currentposition = (NUM_LEDS + currentposition + 8) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(BTN_DOWN)) {
            currentposition = (NUM_LEDS + currentposition - 8) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(BTN_LEFT)) {
            currentposition = (NUM_LEDS + currentposition + 1) % NUM_LEDS;
            buttonPressed = 1;
        }
        if (BTN_JUST_PRESSED(BTN_RIGHT)) {
            currentposition = (NUM_LEDS + currentposition - 1) % NUM_LEDS;
            buttonPressed = 1;
        }

        // Compute which row and col your pointer is in now
        uint8_t row = currentposition / GRID_COLS;
        uint8_t col = currentposition % GRID_COLS;

        if (BTN_JUST_PRESSED(BTN_ENTER)) {
            // Can only choose level if greenLed is selected
            if (row == 0) {
                brightnessDivisor = col + 1; // Higher column means dimmer (max is 7)
                printf("Brightness divisor set to %d\n", brightnessDivisor);
                divisorChosen = true;
            }
            else {
                printf("Please choose brightness level from one of the green LED");
            }
        }

        if (BTN_JUST_PRESSED(BTN_9)) {
            printf("Exit brightness selection without change\n");
            divisorChosen = true;
        }

        updateMoveButton();

        // Show and print the BRIGHTNESS_CONTROL state
        // Only works after user move the pointer one by one
        if (buttonPressed == 1) {
            createBrightnessSelectionScreen();
            buttonPressed = 0;
        }
    }

    // Set out each brightness intensity of each color
    // Need to affect the saved color of each LED
    // for (int i = 0; i < NUM_LEDS; i++) {
    //     if (currentPage == PAINTING_SPACE) {
    //         setColorLEDScaled(i, savedColor[i], brightnessDivisor);
    //     }
    //     else if (currentPage == CODING_SPACE) {
    //         setColorLEDScaled(
    //             i, wholeCodeCanvas[currentCanvas][i].currentColor, brightnessDivisor);
    //     }
    // }

    // Change brightness level of foreground and background color
    
    // Handle brightness level of this somewhere else
    // setColorScaled(foregroundColor, brightnessDivisor);
    // setColorScaled(backgroundColor, brightnessDivisor);

    // Return page to previous screen
    // Set out each brightness intensity of each color
    // Need to affect the saved color of each LED
    if (currentPage == PAINTING_SPACE) {
        // Draw real painting canvas data with brightness applied
        for (int i = 0; i < NUM_LEDS; i++) {
            setColorLEDScaled(i, savedColor[i], brightnessDivisor);
        }
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    }
    else if (currentPage == CODING_SPACE) {
        renderCodingCanvas();
    }
    // Prevent high usage of CPU by sleeping for a short duration before check again
    Delay_Ms(100);
    divisorChosen = false;
    currentPage = prevPageState;
}