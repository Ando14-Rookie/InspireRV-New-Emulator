#include "./binary_game.h"

// Function & variable prototype declaration
// Randomize question number
static int randomNumber;    // Returns 0 to 15
static int8_t currentRound; // Ranges from 1 to 5 (ideally 0 to 4)
// Check if user has answered correctly or just wanna quit the game
static bool stopPlaying;
static inline void renderBinaryGame(uint8_t selectedNumber);
static inline bool checkUserInputCol(uint8_t rowUser[8]);
static inline void renderUserInput(void);
static inline void handleScenario(uint8_t idx);

// By default, game state is BINARY_GAME_IDLE
GameState currentGame;

void initBinaryGame(void) {
    // Check if user has answered correctly or just wanna quit the game
    stopPlaying = false;
    // Ensure that the graph result is only rendered once
    bool graphRendered = false;
    // Only move when pointer moves or user select something
    int buttonPressed = 0;
    // In the beginning it starts at round 1 (ideally 0)
    currentRound = 0;
    // By default, game state is BINARY_GAME_IDLE
    currentGame = BINARY_GAME_IDLE;
    // Seed the random number generator using the current time
    srand(time(NULL));
    // Randomize question number
    randomNumber = rand() % 16; // Returns 0 to 15

    printf("Random number generated is %d \n", randomNumber);

    // Make all LED off first in `led_array`
    fill_color(offColor);

    // As soon as user enter this program, print binary game
    renderBinaryGame(randomNumber);

    // The game will keep running until user get the answer correct, unless
    // they wish to stop the game
    while (!stopPlaying) {

        // Activate keyboard I, J, K, L press input
        checkMoveButton();

        // Navigate pointers
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
        if (BTN_JUST_PRESSED(Enter_Key)) {
            currentGame = GAME_INPUT_CONFIRM;
        }

        // Compute which row and col your pointer is in now
        uint8_t row = currentposition / GRID_COLS;
        uint8_t col = currentposition % GRID_COLS;

        // Update to compare button released and pressed state
        updateMoveButton();

        // Handle the graph in the end of the 5 game round
        if(currentGame == BINARY_GAME_GRAPH){
            if(!graphRendered){
                // Show the graph momentarily until pointer has moved
                flashGameComplete();
                renderResultsGraph();
                graphRendered = true;
            }   
            else if(buttonPressed == 1){
                // Stop playing and go back to the previous page, namely
                // `PAINTING_SPACE`
                currentPage = prevPageState;
                currentGame = BINARY_GAME_IDLE;
                stopPlaying = true;
            }
        }

        // Only works after user move the pointer one by one
        if ((currentGame == BINARY_GAME_IDLE) && (!stopPlaying) && buttonPressed == 1) {
            // Render normally
            renderBinaryGame(randomNumber);
            // printf("currentGame is %d \n", currentGame);
            // Reset state
            buttonPressed = 0;
        }

        // Check what is being pressed
        if (row == 1 && currentGame == GAME_INPUT_CONFIRM) {
            // Undo the flip so it matches rowOneHandle indexing
            uint8_t logicalCol = 7 - col;
            // Handle each scenario
            handleScenario(logicalCol);
            // Continue rendering normally
            renderBinaryGame(randomNumber);

            // Move to next round, Randomize next question & Reset everything to initial
            // state
            if (currentGame == BINARY_GAME_CONTINUE &&
                (roundStatus[currentRound] != ROUND_RETRY)) {
                printf("------------------------- \n");
                printf(" Move to the next round !! \n");
                printf("------------------------- \n");

                // Seed the random number generator using the current time
                srand(time(NULL));
                // Randomize the question again
                randomNumber = rand() % 16; // Returns 0 to 15

                // Reset user input values
                for (int i = 0; i <= 7; i++) {
                    if (rowOneHandle[i] == 1) {
                        rowOneHandle[i] = 0;
                        // Default state = purple color
                        setColorLEDScaled(i, normalColor, brightnessDivisor);
                    }
                }

                currentRound += 1;
                currentGame = BINARY_GAME_IDLE;

                // Render normally
                renderBinaryGame(randomNumber);
            }

            // Reset current game progress to default after whether go to next round or
            // not
            currentGame = BINARY_GAME_IDLE;
        }

        // As soon as the round has gone through 5 times, immediately show the result graph
        if ((currentGame == BINARY_GAME_IDLE) && currentRound > 4) {
            // Continue to graph screen
            currentGame = BINARY_GAME_GRAPH;
            // Stop the while loop
            // stopPlaying = true;
        }

    }
    // Reset user input value and round-status LEDs back to the saved canvas
    for (int i = 0; i <= 7; i++) {
        // Reset roundStatus and roundEntered value
        if (i < 5) {
            roundStatus[i] = ROUND_IDLE;
            roundEntered[i] = 0;
        }
    }

    // Reset the game state (2nd time)
    currentGame = BINARY_GAME_IDLE;

    // As soon as the function stop, render back the real saved canvas
    for (int i = 0; i < NUM_LEDS; i++) {
        setColorLEDScaled(i, savedColor[i], brightnessDivisor);
    }

    // Draw pointer ON TOP visually (only affects led_array), doesn't touch savedColor
    set_color(currentposition, pointerColor);
    printf("Pointer current position is %d\n ", currentposition);

    // Print the emulator screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/**
 * @brief  Show and print the `BINARY_GAME` screen everytime pointer moves or answered
 * @param selectedNumber Which random binary question to be shown
 **/
static inline void renderBinaryGame(uint8_t selectedNumber) {
    // Type of color that should be turned on
    // val can be 0, 1, 2, 3
    int val = -1;

    // Erase old screen, make it all black
    fill_color(offColor);
    // Handle the Row 1 & Row 0
    renderUserInput();
    renderGameRounds();

    // Draw the hardcoded question on rows 1-7
    for (int arrayRow = 0; arrayRow < 5; arrayRow++) {
        // array[0] = bottom = LED row 7, so flip the row
        int ledRow = 7 - arrayRow;
        for (int arrayCol = 0; arrayCol < 8; arrayCol++) {
            // col[0] = rightmost = LED col 7, so flip the col
            int ledCol = 7 - arrayCol;

            // Get the saveConfirmLogo[slotIndex][7][7] or the 64th LED as
            // starting point
            int idx = ledRow * 8 + ledCol;

            // Get the number state of that LED
            val = slotLogo[selectedNumber][arrayRow][arrayCol];

            // Handle which LED needs to be turned ON based on slotLogo and pointer
            if (val == 1)
                // set_color(idx, pointerColor);
                setColorLEDScaled(idx, defaultLogoColor, brightnessDivisor);
        }
    }

    // Draw pointer ON TOP visually (only effect led_array), doesn't touch
    set_color(currentposition, pointerColor);

    // Print the emulator screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/**
 * @brief Handle the user input, confirm button and quit button
 * @param idx Current position in terms of column, range for column is `0 to 7`
 **/
static inline void handleScenario(uint8_t idx) {
    // Handle User Input by changing to selected
    // Only works for user input
    if (idx >= 2 && idx <= 5) {
        if (rowOneHandle[idx] == 0) {
            // Debug
            // printf("Current rowOne is %d \n", rowOneHandle[index]);

            // Change state to selected
            rowOneHandle[idx] = 1;
        }
        else {
            // Change state to unselected
            rowOneHandle[idx] = 0;
        }
    }
    // Handle the quit button
    else if (idx == 0) {
        printf("QUIT - Stop Playing Binary Game! \n");
        // Stop playing and go back to the previous page, namely `PAINTING_SPACE`
        currentPage = prevPageState;
        // Stop the while loop
        stopPlaying = true;
    }
    // Handle the confirm button
    else if (idx == 7 && currentRound <= 4) {
        // Handle the confirm button
        if (checkUserInputCol(rowOneHandle)) {
            // Change current game state
            currentGame = BINARY_GAME_CONTINUE;
            // Play the short right answer animation with sound here
            flashCorrect();
            printf("CORRECT - Your answer matches the corresponding decimal value! \n");
        }
        else {
            // Change current game state
            currentGame = BINARY_GAME_IDLE;
            // Play the short wrong answer animation with sound here
            flashWrong();
            printf("WRONG - Well played, try again! \n");
        }

        // Handle whether should stay/proceed to the next round
        handleGameRounds(checkUserInputCol(rowOneHandle), currentRound);
    }
}

/**
 * @brief Call this everytime emulator screen is printed. This print the user input
 * , confirm button and quit button
 **/
static inline void renderUserInput(void) {
    for (int8_t col = 7; col >= 0; col--) {
        // flip to match physical LED ordering
        int ledCol = 7 - col;
        uint8_t idx = 1 * VERTICAL_BUTTONS + ledCol;

        // Columns 2-5 is always purple (default state)
        // If it is selected, it becomes yellow
        if (rowOneHandle[col] == 0) {
            // Default state = purple color
            setColorLEDScaled(idx, normalColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == 1) {
            // input button selected state = yellow color
            setColorLEDScaled(idx, selectedColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == -1) {
            // Quit button state = red color
            setColorLEDScaled(idx, returnColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == -2) {
            // Confirm button state = green color
            setColorLEDScaled(idx, confirmColor, brightnessDivisor);
        }
        else if (rowOneHandle[col] == 3) {
            // Unused LEDs
            setColorLEDScaled(idx, offColor, brightnessDivisor);
        }
    }
}

/**
 * @brief Handle the logic when user clicks a page button in Row 0
 * @param rowUser The selected column from Row 1
 **/
static inline bool checkUserInputCol(uint8_t rowUser[8]) {
    // col[0] is rightmost; col[7] is leftmost

    /* HOW DOES THE CODE WORK??

    It only shifts the literal number 1,  a completely separate, brand-new value, to build
    the temporary 8-bit binary (0b). The array is only ever read (via rowUser[i]), never
    shifted.

    | i | rowUser[i] | shift = 5-i | Truthy? | Expression    | v before | v after |
    | - | --------   | ----------- | ------- | ------------- | -------- | ------- |
    | 2 | 0          | 3           | false   | 0             | 0000     | 0000    |
    | 3 | 0          | 2           | false   | 0             | 0000     | 0000    |
    | 4 | 1          | 1           | true    | 1 << 1 = 0010 | 0000     | 0010    |
    | 5 | 1          | 0           | true    | 1 << 0 = 0001 | 0010     | 0011    |
    */

    uint8_t v = 0;
    for (int i = 2; i <= 5; i++) {
        int shift = 5 - i; // index 2 -> shift 3, index 5 -> shift 0
        v |= (rowUser[i] ? (1 << shift) : 0);
    }

    // Compare if user input and binary question is the same or not
    return (v == randomNumber);
}
