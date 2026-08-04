#include "addition_game.h"

// Check if user has answered correctly or just wanna quit the game
static bool stopPlaying;
// Round ranges from 1 to 5 (ideally 0 to 4)
static int8_t currentRound; 

// Check how many times the question in each round causes an overflow
uint8_t overflowOccur = 0;

AdditionState currentAddGame;

// `0` means user input; If turn into `1`, it becomes selected; `-1` means unchangeable
static int8_t rowThreeHandle[8] = {-1, 0, 0, 0, 0, 0, -1, -1};

// `-1` refers to leave button; `-2` refers to confirm button; 
static int8_t handleQuitConfirm[8] = {-1, -1, 0, 0, 0, 0, -2, -2};

// Function Prototype/Declaration
static inline void renderAdditionGame(uint8_t questionA, uint8_t questionB);
static inline void drawNibble(uint8_t value, int ledRow, int colOffset);
static inline void renderQuestion(uint8_t questionA, uint8_t questionB);
static inline void renderUserInput(void);
static inline void handleScenario(uint8_t row, uint8_t idx, uint8_t questionA, uint8_t questionB);
static inline bool checkUserInputCol(uint8_t questionA, uint8_t questionB, int8_t rowUser[8]);

void initAdditionGame(void) {
    // Check if user has answered correctly or just wanna quit the game
    stopPlaying = false;
    // Ensure that the graph result is only rendered once
    bool graphRendered = false;
    // By default, game state is BINARY_GAME_IDLE
    currentAddGame = ADDITION_GAME_IDLE;
    // Only move when pointer moves or user select something
    int buttonPressed = 0;
    // In the beginning it starts at round 1 (ideally 0)
    currentRound = 0;

    // Seed the random number generator
    srand(time(NULL));

    // Randomize question number
    uint8_t numberA = rand() % 16; // 0 to 15
    uint8_t numberB = rand() % 16; // 0 to 15

    // Check if 1st round questions will cause overflow or not
    if(numberA + numberB > 15){
        overflowOccur += 1;
        printf("Question at round %d will cause overflow.\n", currentRound);
    }

    // Make all LED off first in `led_array`
    fill_color(offColor);

    // As soon as user enter this program, print binary game
    renderAdditionGame(numberA, numberB);

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

        // Compute which row and col your pointer is in now
        uint8_t row = currentposition / GRID_COLS;
        uint8_t col = currentposition % GRID_COLS;

        if (BTN_JUST_PRESSED(Enter_Key) && (row == 3 || row == 1)) {
            currentAddGame = ADDITION_INPUT_CONFIRM;
        }

        // Update to compare button released and pressed state
        updateMoveButton();

        // Handle the graph in the end of the 5 game round
        if(currentAddGame == ADDITION_GAME_GRAPH){
            if(!graphRendered){
                // Show the graph momentarily until pointer has moved
                flashGameComplete();
                renderOverflowGraph();
                printf("It has overflown by %d times. \n", overflowOccur);
                graphRendered = true;
            }   
            else if(buttonPressed == 1){
                // Stop playing and go back to the previous page, namely
                // `PAINTING_SPACE`
                currentPage = prevPageState;
                stopPlaying = true;
            }
        }

        // Only works after user move the pointer one by one
        if ((currentAddGame == ADDITION_GAME_IDLE) && buttonPressed == 1) {
            // Render normally
            renderAdditionGame(numberA, numberB);
            // printf("currentGame is %d \n", currentGame);
            // Reset state
            buttonPressed = 0;
        }

        // Check what is being pressed
        if ((row == 3 || row == 1) && currentAddGame == ADDITION_INPUT_CONFIRM) {
            // Undo the flip so it matches rowOneHandle indexing
            uint8_t logicalCol = 7 - col;
            // Handle each scenario
            handleScenario(row, logicalCol, numberA, numberB);
            // Continue rendering normally
            renderAdditionGame(numberA, numberB);

            // Move to next round, Randomize next question & Reset everything to 
            // initial state
            if (currentAddGame == ADDITION_GAME_CONTINUE &&
                (roundStatus[currentRound] != ROUND_RETRY)) {
                printf("------------------------- \n");
                printf(" Move to the next round !! \n");
                printf("------------------------- \n");
                
                // Seed the random number generator using the current time
                srand(time(NULL));
                // Randomize the question again
                numberA = rand() % 16; // Returns 0 to 15
                numberB = rand() % 16; // Returns 0 to 15

                // Reset user input values
                for (int i = 0; i <= 7; i++) {
                    if (rowThreeHandle[i] == 1) {
                        rowThreeHandle[i] = 0;
                        // Default state = purple color
                        setColorLEDScaled(i, normalColor, brightnessDivisor);
                    }
                }

                currentRound += 1;
                currentAddGame = ADDITION_GAME_IDLE;

                // TODO NEXT: CHECK IF THE HARDWARE CODE WORKS
                // ENSURE TO INTEGRATE BOTH BINARYGAME AND ADDITION GAME TOGETHER

                // Check if next round questions from Round 1 to 4 will cause overflow or not
                // Round 0 has been handled in the beginning
                if(currentRound < 5){
                    
                    if(numberA + numberB > 15){
                        overflowOccur += 1;
                        printf("Question at round %d will cause overflow.\n", currentRound);
                    }

                    // Render normally
                    renderAdditionGame(numberA, numberB);
                }
            }
            // Reset current game progress to default after whether go to next round or
            // not
            currentAddGame = ADDITION_GAME_IDLE;
        }

        // As soon as the round has gone through 5 times, immediately show the result graph
        if ((currentAddGame == ADDITION_GAME_IDLE) && currentRound > 4) {
            // Continue to graph screen
            currentAddGame = ADDITION_GAME_GRAPH;
            // Stop the while loop
            // stopPlaying = true;
        }
    }
    
    // Reset user input value and round-status LEDs back to the saved canvas
    for (int i = 0; i < 5; i++) {
        // Reset roundStatus and roundEntered value
        roundStatus[i] = ROUND_IDLE;
        roundEntered[i] = 0;
    }

    // Reset the game state
    currentAddGame = ADDITION_GAME_IDLE;
    // Reset the overflow counter
    overflowOccur = 0;

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
 * @brief Handle the user input, confirm button and quit button
 * @param row Which row the pointer is at right now
 * @param idx Current position in terms of column, range for column is `0 to 7`
 * @param questionA The 4-bits binary question 1
 * @param questionB The 4-bits binary question 2
 **/
static inline void handleScenario(uint8_t row, uint8_t idx, uint8_t questionA, uint8_t questionB) {
    #ifdef DEBUG_VERBOSE
    printf("handleScenario called with row=%d idx=%d\n", row, idx);
    #endif
    
    // Handle User Input by changing to selected
    // Only works for user input
    if (row == 3 && idx >= 1 && idx <= 5) {
        if (rowThreeHandle[idx] == 0) {
            // Debug
            // printf("Current rowOne in %d is %d \n", idx, rowThreeHandle[idx]);

            // Change state to selected
            rowThreeHandle[idx] = 1;
        }
        else {
            // Change state to unselected
            rowThreeHandle[idx] = 0;
        }
    }
    // Handle the quit button
    else if (row == 1 && (idx == 0 || idx == 1)) {
        printf("QUIT - Stop Playing Addition Game! \n");
        // Stop playing and go back to the previous page, namely `PAINTING_SPACE`
        currentPage = prevPageState;
        // Stop the while loop
        stopPlaying = true;
    }
    // Handle the confirm button
    else if (row == 1 && (idx == 6 || idx == 7) && currentRound <= 4) {
        // Handle the confirm button
        if (checkUserInputCol(questionA, questionB, rowThreeHandle)) {
            // Change current game state
            currentAddGame = ADDITION_GAME_CONTINUE;
            // Play the short right answer animation with sound here
            flashCorrect();
            printf("CORRECT - Your answer matches the corresponding decimal value! \n");
        }
        else {
            // Change current game state
            currentAddGame = ADDITION_GAME_IDLE;
            // Play the short wrong answer animation with sound here
            flashWrong();
            printf("WRONG - Well played, try again! \n");
        }

        // Handle whether should stay/proceed to the next round
        // Use the one in "../binary_game/multiple_round.c"
        handleGameRounds(checkUserInputCol(questionA, questionB, rowThreeHandle), currentRound);
    }
}

/**
 * @brief  Helper function to draw the binary bits of each question
 * @param value The question number
 * @param ledRow Which row these LED should the question be set
 * @param colOffset How much offset needed to set the LED correctly
 **/
static inline void drawNibble(uint8_t value, int ledRow, int colOffset) {
    for (int bit = 0; bit < 4; bit++) {
        // bit 0 = MSB, bit 3 = LSB
        int val = (value >> (3 - bit)) & 1;
        uint8_t idx = ledRow * 8 + (colOffset - bit);
        setColorLEDScaled(idx, val ? orangeColor : lightOrangeColor, brightnessDivisor);
    }
}

/**
 * @brief  Show and print the `ADDITION_GAME` screen everytime pointer moves or answered
 * @param questionA The 4-bits binary question 1
 * @param questionB The 4-bits binary question 2
 **/
static inline void renderAdditionGame(uint8_t questionA, uint8_t questionB) {
    // Erase old screen, make it all black
    fill_color(offColor);

    // Handle the Row 7, 6, 5, 4 for showing carry and questions
    renderQuestion(questionA, questionB);

    // Handle Row 3, 1 for user inputs
    renderUserInput();

    // Handle Row 0 for rounds
    // Use the one in "../binary_game/multiple_round.c"
    renderGameRounds();

    // Draw pointer ON TOP visually (only effect led_array), doesn't touch
    set_color(currentposition, pointerColor);

    // Print the emulator screen
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/**
 * @brief Call this everytime emulator screen is printed. This prints the question
 * and carry number
 **/
static inline void renderQuestion(uint8_t questionA, uint8_t questionB) {
    // Draw operand nibbles, right-aligned in columns 4-7
    drawNibble(questionA, 6, 5);
    drawNibble(questionB, 5, 5);   // consider drawNibble(questionB, 5, 4) if B belongs here

    // Decorative overlay spots, per row
    static const uint8_t row7Spots[] = {1, 2, 3, 4, 5, 6};
    for (int i = 0; i < 6; i++) {
        setColorLEDScaled(7 * 8 + row7Spots[i], magentaColor, brightnessDivisor);
    }

    setColorLEDScaled(6 * 8 + 6, magentaColor, brightnessDivisor);
    setColorLEDScaled(6 * 8 + 1, magentaColor, brightnessDivisor);

    setColorLEDScaled(5 * 8 + 6, magentaColor, brightnessDivisor);
    setColorLEDScaled(5 * 8 + 1, onColorBlue, brightnessDivisor);

    setColorLEDScaled(4 * 8 + 6, magentaColor, brightnessDivisor);
    setColorLEDScaled(4 * 8 + 5, magentaColor, brightnessDivisor);
    setColorLEDScaled(4 * 8 + 4, magentaColor, brightnessDivisor);
    setColorLEDScaled(4 * 8 + 3, magentaColor, brightnessDivisor);

    for (int col = 0; col <= 2; col++) {
        setColorLEDScaled(4 * 8 + col, onColorBlue, brightnessDivisor);
    }

    setColorLEDScaled(3 * 8 + 1, onColorBlue, brightnessDivisor);
}

/**
 * @brief Call this everytime emulator screen is printed. This prints the user input
 * , confirm button and quit button
 **/
static inline void renderUserInput(void) {
    for (int8_t col = 7; col >= 0; col--) {
        // Flip to match physical LED ordering
        int ledCol = 7 - col;
        // For the 3rd row: user input
        uint8_t idx = 3 * VERTICAL_BUTTONS + ledCol;
        // For the 1st row: quit and confirm button
        uint8_t idxSecond = 1 * VERTICAL_BUTTONS + ledCol;

        // Columns 2-5 is always purple (default state)
        // If it is selected, it becomes yellow
        if (rowThreeHandle[col] == 0) {
            // Default state = purple color
            setColorLEDScaled(idx, normalColor, brightnessDivisor);
        }
        else if (rowThreeHandle[col] == 1) {
            // input button selected state = yellow color if not having overflow; else is red
            setColorLEDScaled(idx, (col == 1)? overflowColor :selectedColor, brightnessDivisor);
            printf("row three led \n");
        }

        // For leave/quit button
        if (handleQuitConfirm[col] == -1){
            setColorLEDScaled(idxSecond, overflowColor, brightnessDivisor);
        }
        // For confirm/enter button
        else if(handleQuitConfirm[col] == -2){
            setColorLEDScaled(idxSecond, confirmColor, brightnessDivisor);
        }
    }
}

/**
 * @brief Handle the logic when user clicks a page button in Row 0
 * @param questionA The 4-bits binary question 1
 * @param questionB The 4-bits binary question 2
 * @param rowUser The selected column from Row 3
 **/
static inline bool checkUserInputCol(uint8_t questionA, uint8_t questionB, int8_t rowUser[8]) {
    // col[0] is rightmost; col[7] is leftmost

    /* HOW DOES THE CODE WORK??

    It only shifts the literal number 1,  a completely separate, brand-new value, to build
    the temporary 8-bit binary (0b). The array is only ever read (via rowUser[i]), never
    shifted.

    | i | rowUser[i] | shift = 5-i | Truthy? | Expression     | v before  | v after  |
    | - | --------   | ----------- | ------- | -------------  | --------  | -------  |
    | 1 | 0          | 4           | false   | 0              | 00000     | 00000    |
    | 2 | 1          | 3           | true    | 1 << 3 = 01000 | 00000     | 01000    |
    | 3 | 0          | 2           | false   | 0              | 01000     | 01000    |
    | 4 | 1          | 1           | true    | 1 << 1 = 00010 | 01000     | 01010    |
    | 5 | 1          | 0           | true    | 1 << 0 = 00001 | 01010     | 01011    |
    */

    uint8_t v = 0;
    for (int i = 1; i <= 5; i++) {
        int shift = 5 - i; // index 1 -> shift 4, index 5 -> shift 0
        v |= (rowUser[i] ? (1 << shift) : 0);
    }

    // Compare if user input and binary question is the same or not
    return (v == (questionA + questionB));
}