#include "painting_space.h"

// Declaration
static inline void index_to_rc(int, int*, int*);
static inline int rc_to_index(int, int);
static inline bool color_equals(color_t a, color_t b);

// Definition
color_t foregroundColor = {100, 255, 100}; // Green
color_t backgroundColor = {100, 100, 255}; // Blue

color_t scaledForeground;
color_t scaledBackground;

color_t tempColor = {0, 0, 0}; // Container for bucket fill feature
color_t slotColor = {100, 100, 255}; // Also blue
color_t returnColor = {255, 0, 0};  // red 

// Will be used in this file only
// true = foreground, false = background
bool pickingForeground = true;

void initScaledForeBackColors(void) {
    scaledForeground = initColorScaled(foregroundColor, brightnessDivisor);
    scaledBackground = initColorScaled(backgroundColor, brightnessDivisor);
}

void bucketFill(int startIndex, color_t fillColor) {
    // Get the color of the real LED color in that position
    color_t targetColor = savedColor[startIndex];

    // If already the fill color, nothing to do
    if (color_equals(targetColor, fillColor))
        return;

    // To-do list of LED indices that still need to be processed
    int stack[NUM_LEDS];
    int top = 0;
    // Tracks which LEDs have already been added to the stack
    bool visited[NUM_LEDS] = {false};
    // Starting LED is pushed onto the stack first
    stack[top++] = startIndex;
    // First starting point has been processed
    visited[startIndex] = true;

    // Keeps running as long as there are LEDs left to process
    while (top > 0) {
        // Pops the most recently added LED off from top stack
        int current = stack[--top];
        int row, col;
        // Convert 1D index to 2D row/col
        index_to_rc(current, &row, &col); // Ex: Index 17 → row 2, col 1

        // Paint this LED in both arrays
        savedColor[current] = fillColor;
        set_color(current, fillColor);

        // Check all 4 neighbors of that specific LED
        int neighbors[4][2] = {
            {row - 1, col}, {row + 1, col}, {row, col - 1}, {row, col + 1}};

        for (int i = 0; i < 4; i++) {
            int nr = neighbors[i][0]; // neighbor row
            int nc = neighbors[i][1]; // neighbor column

            // Case when out of boundary
            if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8)
                continue;
            // Turn back 2D to 1D array such as "(2, 3) becomes 2 * 8 + 3 = 19"
            int neighborIndex = rc_to_index(nr, nc);

            // True if the LED neighbour has not been visited and it has the same \
            original color as the region being filled. Only
            if (!visited[neighborIndex] &&
                color_equals(savedColor[neighborIndex], targetColor)) {
                // Only fill the area where the current spot is the same as the color
                // where user select to fill
                visited[neighborIndex] = true;
                // The LED that still needs to be processed is added to the top of stack
                stack[top++] = neighborIndex;
            }
        }
    }
}

void fullBucketFillLogic(void) {
    // Color picker block — inside handleButtonFunction
    bool newColorChosen = false;
    int buttonPressed = 0;
    // If pressed once, it will exit colorMap; If pressed the 2nd time, it will bucket
    // fill color
    int pressedEnter = 0;
    // Monitor which color that is selected for bucket fill
    color_t selected = colorfulMap[currentposition];

    // One-time setup
    initColorfulMap();
    fill_color(offColor);
    Delay_Ms(100);
    clearScreen();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)colorfulMap, NUM_LEDS * 3);

    // Reset prev states once before the loop
    updateMoveButton();

    while (!newColorChosen) {
        // Activate keyboard I, J, K, L press input
        checkMoveButton();

        // Phase 1: navigating colorfulMap to pick a color
        if (pressedEnter == 0) {
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

            // 1st enter: confirm color, return to canvas
            if (BTN_JUST_PRESSED(BTN_ENTER)) {
                selected = colorfulMap[currentposition];
                tempColor = selected;
                pressedEnter = 1;
                printf("Bucket fill color selected: {%d, %d, %d}\n", selected.g,
                    selected.r, selected.b);

                // Restore canvas
                for (int i = 0; i < NUM_LEDS; i++){
                    // set_color(i, savedColor[i]);
                    setColorLEDScaled(i, savedColor[i], brightnessDivisor);
                }
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            }

            // Draw colorfulMap + pointer while navigating
            if (buttonPressed == 1) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    set_color(i, colorfulMap[i]);
                }
                set_color(currentposition, pointerColor);
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
                buttonPressed = 0;
            }
        }
        // Phase 2: navigating canvas to pick where to fill
        else if (pressedEnter == 1) {
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

            // 2nd enter: run bucket fill from current position, exit loop
            if (BTN_JUST_PRESSED(BTN_ENTER)) {
                bucketFill(currentposition, tempColor);

                // Show updated canvas
                for (int i = 0; i < NUM_LEDS; i++){
                    // set_color(i, savedColor[i]);
                    setColorLEDScaled(i, savedColor[i], brightnessDivisor);
                }
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

                currentPage = PAINTING_SPACE;
                pressedEnter = 0;
                newColorChosen = true; // ← only exits here, after fill is done
                printf("Changed pageState back to %d\n", currentPage);
            }

            // Draw canvas + pointer while navigating fill position
            if (buttonPressed == 1) {
                for (int i = 0; i < NUM_LEDS; i++){
                    // set_color(i, savedColor[i]);
                    setColorLEDScaled(i, savedColor[i], brightnessDivisor);
                }
                set_color(currentposition, pointerColor);
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
                buttonPressed = 0;
            }
        }

        // Update prev to current pressed button
        // Used for comparison determine whether a key is pressed/just released
        updateMoveButton();

        Delay_Ms(100);
    }
    // Confirmation that all value is reset to default
    buttonPressed = 0;
    pressedEnter = 0;
}

void chooseColorForeBack(void) {
    static bool newColorChosen = false;
    static int buttonPressed = 0;

    // Reset joystick prev states so first tick is clean
    updateMoveButton();

    // Create the colorMap
    initColorfulMap();
    // Set each LED color to black
    // Set everything to 0
    fill_color(offColor);
    Delay_Ms(100);
    clearScreen();

    // Print out the colorfulMap once before entering the loop condition
    WS2812BSimpleSend(LED_PINS, (uint8_t *)colorfulMap, NUM_LEDS * 3);

    while (!newColorChosen) {
        // Read ALL inputs inside this loop
        checkMoveButton();

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
            color_t selected = colorfulMap[currentposition];
            if (pickingForeground) {
                foregroundColor = selected;
                printf("Foreground color updated to : {%d, %d, %d}\n", selected.g,
                    selected.r, selected.b);
            }
            else {
                backgroundColor = selected;
                printf("Background color updated: {%d, %d, %d}\n", selected.g, selected.r,
                    selected.b);
            }
            // Return led_array to the correct saved color array
            for (int i = 0; i < NUM_LEDS; i++) {
                // set_color(i, savedColor[i]);
                setColorLEDScaled(i, savedColor[i], brightnessDivisor);
            }
            WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            // Change the pagestate back to PAINTING_SPACE
            currentPage = PAINTING_SPACE;
            // This will print the id of 0 for PAINTING_SPACE
            printf("Change pageState back to %d", currentPage);
            newColorChosen = true;
            buttonPressed = 0;
        }

        // Update ALL prev states inside this loop
        updateMoveButton();

        if (buttonPressed == 1) {
            // Momentarily set led_array to colorfulMap then print the emulator
            for (int i = 0; i < NUM_LEDS; i++) {
                set_color(i, colorfulMap[i]);
            }
            // Momentarily set the current position to pointerColor
            set_color(currentposition, pointerColor);
            WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            buttonPressed = 0;
        }

        Delay_Ms(100);
    }
    // Reset back to default
    newColorChosen = false;
}

static uint8_t lerp_u8(uint8_t initVal, uint8_t endVal, uint8_t row) {
    return (uint8_t)(initVal + ((endVal - initVal) * row) / 7);
}

void initColorfulMap(void) {
    color_t base[8] = {
        {0,   0,   255},  // col 0 = blue
        {0,   200, 255},  // col 1 = turquoise
        {0,   255, 0},    // col 2 = green
        {255, 255, 0},    // col 3 = yellow
        {255, 140, 0},    // col 4 = orange
        {255, 0,   0},    // col 5 = red
        {180, 0,   255},  // col 6 = purple
        {0,   0,   0}     // col 7 = black (special gray column above row 0)
    };

    for (int row = 0; row < HORIZONTAL_BUTTONS; row++) {
        for (int col = 0; col < VERTICAL_BUTTONS; col++) {
            // Determine which position each LED is in
            int pos = row * 8 + col;

            if (col == 7) {
                // special grayscale column: black -> white
                uint8_t v = lerp_u8(0, 255, row);
                colorfulMap[pos] = (color_t){v, v, v};
            } else {
                uint8_t r = lerp_u8(base[col].r, 255, row);
                uint8_t g = lerp_u8(base[col].g, 255, row);
                uint8_t b = lerp_u8(base[col].b, 255, row);
                colorfulMap[pos] = (color_t){g, r, b};
            }
        }
    }
}

void updateForegroundColor(color_t foregroundLEDColor){
    scaledForeground = (color_t){foregroundLEDColor.g, foregroundLEDColor.r, foregroundLEDColor.b};
}

void updateBackgroundColor(color_t backgroundLEDColor){
    scaledBackground = (color_t){backgroundLEDColor.g, backgroundLEDColor.r, backgroundLEDColor.b};
}

void updateLEDColor(uint8_t led){
    // Check the ledCondition of that LED;
    // If 0 means OFF, 1 means foreground, 2 means background
    switch (ledCondition[led])
    {
    case 0:
        // printf("Case 0 on LED %d", led);
        // Set the LED to be the normal foreground color first so savedColor can save it later
        set_color(led, foregroundColor);
        // printf("Scaled Foreground is %d, %d, %d \n", scaledForeground.r, scaledForeground.g, scaledForeground.b);
        ledCondition[led] = 1;
        break;
    case 1:
        // printf("Case 1 on LED %d", led);
        // Set the LED to be the the normal foreground color first so savedColor can save it later
        set_color(led, backgroundColor);
        // printf("Scaled Background is %d, %d, %d \n", scaledBackground.r, scaledBackground.g, scaledBackground.b);
        ledCondition[led] = 2;
        break;
    case 2:
        // printf("Case 2 on LED %d", led);
        // Set the LED to empty/black
        set_color(led, defaultColor);
        ledCondition[led] = 0;
        break;
    default:
        printf("Error occured in updating the LED color condition (0/1/2) .....");
        break;
    }
    
}

// Helper functions for the bucket fill feature

// Get User slot index
static inline void index_to_rc(int idx, int * row, int * col) {
    *row = idx / 8;
    *col = idx % 8;
}

static inline int rc_to_index(int row, int col) { return row * 8 + col; }

static inline bool color_equals(color_t a, color_t b) {
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}
