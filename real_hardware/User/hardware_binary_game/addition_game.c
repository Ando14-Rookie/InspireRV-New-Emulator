#include "addition_game.h"

// Check if user has answered correctly or just wanna quit the game
static bool stopPlaying;
// Round ranges from 1 to 5 (ideally 0 to 4)
static int8_t currentRound; 

// Check how many times the question in each round causes an overflow
uint8_t overflowOccur = 0;

AdditionState currentAddGame;

// Grabs two different sources of "unpredictable" data and XORs them together
static uint32_t nextRandom;

// `0` means user input; If turn into `1`, it becomes selected; `-1` means unchangeable
static int8_t rowThreeHandle[8] = {-1, 0, 0, 0, 0, 0, -1, -1};

// `-1` refers to leave button; `-2` refers to confirm button; 
static int8_t handleQuitConfirm[8] = {-1, -1, 0, 0, 0, 0, -2, -2};

// Function Prototype/Declaration
static inline void renderAdditionGame(uint8_t questionA, uint8_t questionB);
static inline void drawNibble(uint8_t value, int ledRow, int colOffset, uint8_t brightnessDivisor);
static inline void renderQuestion(uint8_t questionA, uint8_t questionB);
static inline void renderUserInput(uint8_t brightnessDivisor);
static inline void handleScenario(uint8_t row, uint8_t idx, uint8_t questionA, uint8_t questionB);
static inline bool checkUserInputCol(uint8_t questionA, uint8_t questionB, int8_t rowUser[8]);
static void displayAdditionQuestionHW(uint8_t questionA, uint8_t questionB, uint8_t brightnessDivisor);

void initAdditionGameHW(uint8_t brightnessDivisor) {
    // Store which button is being pressed
    int8_t button = noButtonPressed;
    // Check if user has answered correctly or just wanna quit the game
    stopPlaying = false;
    // Ensure that the graph result is only rendered once
    bool graphRendered = false;
    // By default, game state is BINARY_GAME_IDLE
    currentAddGame = ADDITION_GAME_IDLE;
    // Indicator to determine if the result graph need to be showed once 
    bool quitRequested = false;
    // Only move when pointer moves or user select something
    int buttonPressed = 0;
    // In the beginning it starts at round 1 (ideally 0)
    currentRound = 0;

    // Seed the random number generator
    nextRandom = seedRandomFromHW(nextRandom);

    // Randomize question number
    uint8_t numberA = getRandom0to15(nextRandom); // 0 to 15
    uint8_t numberB = getRandom0to15(nextRandom);; // 0 to 15

    // Check if 1st round questions will cause overflow or not
    if(numberA + numberB > 15){
        overflowOccur += 1;
        printf("Question at round %d will cause overflow.\n", currentRound);
    }
    
    // Make all LED off first in `led_array`
    fill_color(offColor);

    // As soon as user enter this program, print binary game
    displayAdditionQuestionHW(numberA, numberB, brightnessDivisor);

    // The game will keep running until user get the answer correct, unless
    // they wish to stop the game
    while (!stopPlaying && currentRound <= 5){

        if (JOY_9_pressed()) {
            stopPlaying = true;
            quitRequested = true;
            break;
        }

        //  Repeatedly polls/checks the button matrix
        button = matrix_pressed_two();

        // Check if any button is pressed
        // Handle the user input
        if (currentAddGame != ADDITION_GAME_GRAPH && button != noButtonPressed) {
            // Handle quit
            if (button == 15 || button == 14) {
                currentAddGame = ADDITION_GAME_IDLE;
                stopPlaying = true;
                quitRequested = true;
                break;
            }
            else if (button >= 26 && button <= 30) {
                // Ensure that the current button pressed index matches the one in
                // rowOneHandle
                if (rowThreeHandle[7 - (button % horizontalButtons)] == 0) {
                    // Debug
                    // printf("Current rowOne is %d \n", rowOneHandle[index]);

                    // Change state to selected
                    rowThreeHandle[7 - (button % horizontalButtons)] = 1;

                    // DEBUG
                    // printf("Index %d is changed to 1", (7 - (button %
                    // horizontalButtons)));
                }
                else {
                    // Change state to unselected
                    rowThreeHandle[7 - (button % horizontalButtons)] = 0;

                    // #ifdef DEBUG_VERBOSE
                    // printf("Index %d is changed to 0", (7 - (button %
                    // horizontalButtons)));
                    // #endif
                }

                    // Display binary question first time
                    displayAdditionQuestionHW(numberA, numberB, brightnessDivisor);
            }
            // Handle confirm
            else if (button == 8 || button == 9) {
                // Check user input with the corresponding `nextRandom` binary question
                switch (checkUserInputCol(numberA, numberB, rowThreeHandle)) {
                    case (true):
                        flashCorrect();
                        #ifdef DEBUG_TEST
                        printf("CORRECT - Your answer matches the corresponding decimal "
                               "value!");
                        #endif
                        break;
                    case (false):
                        flashWrong();
                        #ifdef DEBUG_TEST
                        printf("WRONG - Well played, try again!");
                        #endif
                }

                // Handle whether should stay/proceed to the next round
                handleGameRounds(checkUserInputCol(numberA, numberB, rowThreeHandle), currentRound, brightnessDivisor);

                // Move to next round, Randomize next question & Reset everything to
                // initial
                // state
                if (currentAddGame == ADDITION_GAME_CONTINUE &&
                    (roundStatus[currentRound] != ROUND_RETRY)) {
                    // printf("------------------------- \n");
                    // printf(" Move to the next round !! \n");
                    // printf("------------------------- \n");

                    // Get random seed
                    nextRandom = seedRandomFromHW(nextRandom);
                    numberA = getRandom0to15(nextRandom);
                    numberB = getRandom0to15(nextRandom);

                    // Reset user input values
                    for (int i = 0; i <= 7; i++) {
                        if (rowThreeHandle[i] == 1) {
                            rowThreeHandle[i] = 0;
                            // Default state = purple color
                            set_color(i, purpleColor, brightnessDivisor);
                        }
                    }
                    // Check if next round questions from Round 1 to 4 will cause overflow or not
                    // Round 0 has been handled in the beginning
                    if(currentRound < 4){
                        if(numberA + numberB > 15){
                            overflowOccur += 1;
                            printf("Question at round %d will cause overflow.\n", (currentRound+1));
                        } 
                    }
                    currentRound += 1;
                    // As soon as the round has gone through 5 times, immediately show the result graph
                    currentAddGame = (currentRound > 4) ? ADDITION_GAME_GRAPH : ADDITION_GAME_IDLE;
                }
                // Keep showing everytime user select/press something, unless quitting the
                // game
                if(currentRound <= 4){
                    displayAdditionQuestionHW(numberA, numberB, brightnessDivisor);
                }
            }
            else if(currentRound <=4){
                // Reset current game progress to default after whether go to next round or
                // not
                currentAddGame = ADDITION_GAME_IDLE;
            }
        }
        // Only run after player has played 5 rounds
        else if (currentAddGame == ADDITION_GAME_GRAPH) {
            if (!graphRendered) {
                // Show the graph momentarily until pointer has moved
                flashGameComplete();
                renderOverflowGraph(overflowOccur, brightnessDivisor);
                graphRendered = true;
            }
            else if(JOY_9_pressed()) {
                // Stop playing and go back to the previous page, namely
                // `PAINTING_SPACE`
                currentAddGame = ADDITION_GAME_IDLE;
                stopPlaying = true;
                quitRequested = true;
            }
        }
        
        Delay_Ms(200);
    }
    if (quitRequested) {
        // Reset user input value
        for (int i = 0; i <= 7; i++) {
            // Reset roundStatus and roundEntered value
            if (i < 5) {
                roundStatus[i] = ROUND_IDLE;
                roundEntered[i] = 0;
            }
            // Reset user input values
            if (rowThreeHandle[i] == 1) {
                rowThreeHandle[i] = 0;
                // Default state = purple color
                set_color(i, purpleColor, brightnessDivisor);
            }
        }
        // Reset all value back to normal
        currentAddGame = ADDITION_GAME_IDLE;
        stopPlaying = false;
        currentRound = 0;
        overflowOccur = 0;
        quitRequested = false;
        // Rerender canvas using real current data
        flushCanvas();
    }
}

/**
 * @brief  Helper function to draw the binary bits of each question
 * @param value The question number
 * @param ledRow Which row these LED should the question be set
 * @param colOffset How much offset needed to set the LED correctly
 **/
static inline void drawNibble(uint8_t value, int ledRow, int colOffset, uint8_t brightnessDivisor) {
    for (int bit = 0; bit < 4; bit++) {
        // bit 0 = MSB, bit 3 = LSB
        int val = (value >> (3 - bit)) & 1;
        uint8_t idx = ledRow * 8 + (colOffset - bit);
        set_color(idx, val ? orangeColor : lightOrangeColor, brightnessDivisor);
    }
}

/**
 * @brief  Show and print the `ADDITION_GAME` screen everytime pointer moves or answered
 * @param questionA The 4-bits binary question 1
 * @param questionB The 4-bits binary question 2
 **/
static void displayAdditionQuestionHW(uint8_t questionA, uint8_t questionB, uint8_t brightnessDivisor) {
    clear();

    // Handle printing of Row of user input, confirm, quit and game round
    renderUserInput(brightnessDivisor);
    renderGameRounds(brightnessDivisor);

    // Logic for printing the screen question
    // Draw operand nibbles, right-aligned in columns 4-7
    drawNibble(questionA, 6, 5, brightnessDivisor);
    drawNibble(questionB, 5, 5, brightnessDivisor);   // consider drawNibble(questionB, 5, 4) if B belongs here

    // Decorative overlay spots, per row
    static const uint8_t row7Spots[] = {1, 2, 3, 4, 5, 6};
    for (int i = 0; i < 6; i++) {
        set_color(7 * 8 + row7Spots[i], magentaColor, brightnessDivisor);
    }

    set_color(6 * 8 + 6, magentaColor, brightnessDivisor);
    set_color(6 * 8 + 1, magentaColor, brightnessDivisor);

    set_color(5 * 8 + 6, magentaColor, brightnessDivisor);
    set_color(5 * 8 + 1, blueColor, brightnessDivisor);

    set_color(4 * 8 + 6, magentaColor, brightnessDivisor);
    set_color(4 * 8 + 5, magentaColor, brightnessDivisor);
    set_color(4 * 8 + 4, magentaColor, brightnessDivisor);
    set_color(4 * 8 + 3, magentaColor, brightnessDivisor);

    for (int col = 0; col <= 2; col++) {
        set_color(4 * 8 + col, blueColor, brightnessDivisor);
    }

    set_color(3 * 8 + 1, blueColor, brightnessDivisor);

    // Send the new led_array to InspireRV
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}


/**
 * @brief Call this everytime emulator screen is printed. This print the user input
 * , confirm button and quit button in row 3 and 1
 **/
static void renderUserInput(uint8_t brightnessDivisor) {
    for (int8_t col = 7; col >= 0; col--) {
        // flip to match physical LED ordering
        int ledCol = 7 - col;
        // For the 3rd row: user input
        uint8_t idx = 3 * verticalButtons + ledCol;
        // For the 1st row: quit and confirm button
        uint8_t idxSecond = 1 * verticalButtons + ledCol;

        switch (rowThreeHandle[col]) {
            case 0:
                set_color(
                    idx, (color_t){100, 165, 255}, brightnessDivisor); // default = purple
                break;
            case 1:
                set_color(idx, (col == 1)? redColor :yellowColor,
                    brightnessDivisor); // selected = yellow
                break;
        }
        
        // For leave/quit button
        if (handleQuitConfirm[col] == -1){
            set_color(idxSecond, redColor, brightnessDivisor);
        }
        // For confirm/enter button
        else if(handleQuitConfirm[col] == -2){
            set_color(idxSecond, greenColor, brightnessDivisor);
        }

        // Send the new led_array to InspireRV
        // WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
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