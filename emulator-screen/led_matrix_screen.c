#include "led_matrix_screen.h"
// #include "ws2812b_simple.h" // The compiler skips the file if it has already been read
// #include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"

uint8_t currentposition = 0;
int buttonPressed = 0;
uint8_t toggle[NUM_LEDS] = {0};

// Determine whether key is still being held or not
// Private to this local file only
// static bool prev_up = false;
// static bool prev_down = false;
// static bool prev_left = false;
// static bool prev_right = false;
// static bool prev_enter = false;

/*
    Key just pressed → cur=true, prev=false → move once ✓
    Key still held → cur=true, prev=true → skip ✓
    Key released → cur=false → skip ✓
*/

void makeEmulatorScreen(color_t * arr) {

    SystemInit();
    // ADC_init(); This does nothing

    // Set each LED color to black
    fill_color(offColor);
    Delay_Ms(100);
    clearScreen();

    // Print Emulator Screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)arr, NUM_LEDS * 3);

    while (isRunning()) {
        checkAllButtons();

        // int act_pressed = JOY_act_pressed();
        // move current position

        // Handle the painting and coding page for each button here
        // Points to variable currentPage that stores the info
        handleButtonFunction();

        // Only runs when simSTATE == SIM_RUNNING, which is set in coding_space.c
        tickStepSimulation();

        if ((currentPage == PAINTING_SPACE || currentPage == CODING_SPACE) && simState != SIM_RUNNING) {
            // Ensure logic only works if key just got pressed and not being hold
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
            if (BTN_JUST_PRESSED(BTN_ENTER)) {
                if (currentPage == PAINTING_SPACE) {
                    printf("PAINTING_SPACE -- You have confirmed'.\n");
                    // Change the led_array and ledCondition in this position
                    updateLEDColor(currentposition);
                    // Snapshot it first before adjusting led_array based on brightness value
                    savedColor[currentposition] =
                        led_array[currentposition]; 
                }
                else if (currentPage == CODING_SPACE) {
                    printf("CODING_SPACE -- You have confirmed'.\n");
                    // Update the LED real data
                    updateCodeLED(currentposition);
                    // Handles somewhere else: Update led_array, then print again the full coding canvas
                }
                buttonPressed = 1;
            }
        }

        // Save current state as previous for next tick
        updateMoveButton();
        // updateAllButtons();

        // Only works after user move the pointer one by one
        if (buttonPressed == 1) {
            if (currentPage == PAINTING_SPACE) {
                // Restore all LEDs from savedColors first (real color)
                // Adjusting led_array based on brightness value
                for (int i = 0; i < NUM_LEDS; i++) {
                    setColorLEDScaled(i, savedColor[i], brightnessDivisor);
                }
                // Draw pointer ON TOP visually (only effect led_array), doesn't touch
                // savedColor (real saved data)
                set_color(currentposition, pointerColor);
                printf("Pointer current position is %d\n ", currentposition);

                // Print Emulator Screen using the visual color data led_array
                WS2812BSimpleSend(LED_PINS, (uint8_t *)arr, NUM_LEDS * 3);
            }
            else if (currentPage == CODING_SPACE) {
                // Restore all LEDs from wholeCodeCanvas first (real color)
                // Update led_array, then print again the full coding canvas
                renderCodingCanvas();
            }
            buttonPressed = 0;
        }
    }

    // Prevent high usage of CPU by sleeping for a short duration before check again
    Delay_Ms(100);
}
