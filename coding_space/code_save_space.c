#include "paint_save_space.h"

// Declaration for static inline
static inline int userSlotIndex();
static inline void createSlotSelectionScreen(int);
static int slotIndex;

color_t codeSaveSlots[SAVE_SLOTS][NUM_LEDS];
bool codeSlotUsed[SAVE_SLOTS] = {false};

// S logo to be shown when choosing where to save
// Variable: `saveLogo[row][column]`
static const int saveLogo[7][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, // row 0: XXXXXXXX
    {0, 0, 1, 1, 1, 1, 1, 0}, // row 1: XX■■■■■X  <- adjusted from your pattern
    {0, 1, 0, 0, 0, 0, 0, 0}, // row 2: X■XXXXXX
    {0, 1, 1, 1, 1, 1, 1, 0}, // row 3: X■■■■■■X  <- adjusted
    {0, 0, 0, 0, 0, 0, 1, 0}, // row 4: XXXXXX■X
    {0, 1, 1, 1, 1, 1, 0, 0}, // row 5: X■■■■■XX  <- adjusted
    {0, 0, 0, 0, 0, 0, 0, 0}, // row 6: XXXXXXXX
};

// It is and 8 type of 8x8 LED matrix scenario
// Variable: `saveConfirmLogo[slotIndex][row][column]`
static const int saveConfirmLogo[8][8][8] =
    {

        // ── Slot 0 ── (S + 0)
        [0] =
            {
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 7: ■■■ X ■■■ X
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 6: ■XX X ■X■ X
                {1, 1, 1, 0, 1, 0, 1, 0}, // row 5: ■■■ X ■X■ X
                {0, 0, 1, 0, 1, 0, 1, 0}, // row 4: XX■ X ■X■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 3: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 2: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 1: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 0: gap
            },

        // ── Slot 1 ── (S + 1)
        [1] =
            {
                {1, 1, 1, 0, 0, 1, 0, 0}, // row 0: ■■■ X X■X X
                {1, 0, 0, 0, 1, 1, 0, 0}, // row 1: ■XX X ■■X X
                {1, 1, 1, 0, 0, 1, 0, 0}, // row 2: ■■■ X X■X X
                {0, 0, 1, 0, 0, 1, 0, 0}, // row 3: XX■ X X■X X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 2 ── (S + 2)
        [2] =
            {
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 0: ■■■ X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 1: ■XX X XX■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 2: ■■■ X ■■■ X
                {0, 0, 1, 0, 1, 0, 0, 0}, // row 3: XX■ X ■XX X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 3 ── (S + 3)
        [3] =
            {
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 0: ■■■ X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 1: ■XX X XX■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 2: ■■■ X ■■■ X
                {0, 0, 1, 0, 0, 0, 1, 0}, // row 3: XX■ X XX■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 4 ── (S + 4)
        [4] =
            {
                {1, 1, 1, 0, 1, 0, 1, 0}, // row 0: ■■■ X ■X■ X
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 1: ■XX X ■X■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 2: ■■■ X ■■■ X
                {0, 0, 1, 0, 0, 0, 1, 0}, // row 3: XX■ X XX■ X
                {1, 1, 1, 0, 0, 0, 1, 0}, // row 4: ■■■ X XX■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 5 ── (S + 5)
        [5] =
            {
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 0: ■■■ X ■■■ X
                {1, 0, 0, 0, 1, 0, 0, 0}, // row 1: ■XX X ■XX X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 2: ■■■ X ■■■ X
                {0, 0, 1, 0, 0, 0, 1, 0}, // row 3: XX■ X XX■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 6 ── (S + 6)
        [6] =
            {
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 0: ■■■ X ■■■ X
                {1, 0, 0, 0, 1, 0, 0, 0}, // row 1: ■XX X ■XX X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 2: ■■■ X ■■■ X
                {0, 0, 1, 0, 1, 0, 1, 0}, // row 3: XX■ X ■X■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 7 ── (S + 7)
        [7] =
            {
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 0: ■■■ X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 1: ■XX X XX■ X
                {1, 1, 1, 0, 0, 0, 1, 0}, // row 2: ■■■ X XX■ X
                {0, 0, 1, 0, 0, 0, 1, 0}, // row 3: XX■ X XX■ X
                {1, 1, 1, 0, 0, 0, 1, 0}, // row 4: ■■■ X XX■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },
};

// L logo to be shown when choosing where to load
static const int loadLogo[7][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, // row 0: XXXXXXXX
    {0, 1, 0, 0, 0, 0, 0, 0}, // row 1: X■XXXXXX  <- adjusted from your pattern
    {0, 1, 0, 0, 0, 0, 0, 0}, // row 2: X■XXXXXX
    {0, 1, 0, 0, 0, 0, 0, 0}, // row 3: X■XXXXXX  <- adjusted
    {0, 1, 0, 0, 0, 0, 0, 0}, // row 4: X■XXXXXX
    {0, 1, 1, 1, 1, 0, 0, 0}, // row 5: X■■■■XXX  <- adjusted
    {0, 0, 0, 0, 0, 0, 0, 0}, // row 6: XXXXXXXX
};

// It is and 8 type of 8x8 LED matrix scenario
static const int loadConfirmLogo[8][8][8] =
    {

        // ── Slot 0 ── (S + 0)
        [0] =
            {
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 7: ■XX X ■■■ X
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 6: ■XX X ■X■ X
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 5: ■XX X ■X■ X
                {0, 0, 0, 0, 1, 0, 1, 0}, // row 4: ■XX X ■X■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 3: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 2: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 1: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 0: gap
            },

        // ── Slot 1 ── (S + 1)
        [1] =
            {
                {1, 0, 0, 0, 0, 1, 0, 0}, // row 0: ■XX X X■X X
                {1, 0, 0, 0, 1, 1, 0, 0}, // row 1: ■XX X ■■X X
                {1, 0, 0, 0, 0, 1, 0, 0}, // row 2: ■XX X X■X X
                {1, 0, 0, 0, 0, 1, 0, 0}, // row 3: ■XX X X■X X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 2 ── (S + 2)
        [2] =
            {
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 0: ■XX X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 1: ■XX X XX■ X
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 2: ■XX X ■■■ X
                {0, 0, 0, 0, 1, 0, 0, 0}, // row 3: ■XX X ■XX X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 3 ── (S + 3)
        [3] =
            {
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 0: ■XX X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 1: ■XX X XX■ X
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 2: ■XX X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 3: ■XX X XX■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 4 ── (S + 4)
        [4] =
            {
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 0: ■XX X ■X■ X
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 1: ■XX X ■X■ X
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 2: ■XX X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 3: ■XX X XX■ X
                {1, 1, 1, 0, 0, 0, 1, 0}, // row 4: ■■■ X XX■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 5 ── (S + 5)
        [5] =
            {
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 0: ■XX X ■■■ X
                {1, 0, 0, 0, 1, 0, 0, 0}, // row 1: ■XX X ■XX X
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 2: ■XX X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 3: ■XX X XX■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 6 ── (S + 6)
        [6] =
            {
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 0: ■XX X ■■■ X
                {1, 0, 0, 0, 1, 0, 0, 0}, // row 1: ■XX X ■XX X
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 2: ■XX X ■■■ X
                {1, 0, 0, 0, 1, 0, 1, 0}, // row 3: ■XX X ■X■ X
                {1, 1, 1, 0, 1, 1, 1, 0}, // row 4: ■■■ X ■■■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },

        // ── Slot 7 ── (S + 7)
        [7] =
            {
                {1, 0, 0, 0, 1, 1, 1, 0}, // row 0: ■XX X ■■■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 1: ■XX X XX■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 2: ■XX X XX■ X
                {1, 0, 0, 0, 0, 0, 1, 0}, // row 3: ■XX X XX■ X
                {1, 1, 1, 0, 0, 0, 1, 0}, // row 4: ■■■ X XX■ X
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 5: gap
                {2, 0, 0, 0, 0, 0, 0, 3}, // row 6: B + O
                {0, 0, 0, 0, 0, 0, 0, 0}, // row 7: gap
            },
};

// void initCodeSaveSlots(void) {
//     for (int s = 0; s < SAVE_SLOTS; s++) {
//         // paintSlotUsed[s] = false;
//         for (int i = 0; i < NUM_LEDS; i++) {
//             codeSaveSlots[s][i] = offColor;
//         }
//     }
// }

void saveOrLoadCodeToSlot(void) {
    // renderSaveLoadSlotScreen function: to print save/load screen emulator and let user
    // choose which slot
    renderCodeSaveLoadSlotScreen();

    // Copy savedColor (the real data) to the paintSaveSlots
    for (int i = 0; i < NUM_LEDS; i++) {
        codeSaveSlots[slotIndex][i] = wholeCodeCanvas[currentCanvas][i].currentColor;
    }
}

void renderCodeSaveLoadSlotScreen(void) {
    int row = 0;             // bottom row
    bool slotChosen = false; // check if user has chosen any slot
    int pressedEnter = 0;
    int printOnce = 0;
    bool returnToSlotPage =
        true; // check if user just return/back to previous slot selection page

    // Wrong Case
    if (slotIndex < 0 || slotIndex >= SAVE_SLOTS)
        return;

    // Reset prev states once before the loop
    updateNineButton();

    // Keeps running until it has been click Enter twice
    while (!slotChosen) {
        // Show and print the PAINTING_SAVE_SLOT or PAINTING_LOAD_SLOT state
        if (returnToSlotPage) {
            createSlotSelectionScreen(row);
            returnToSlotPage = false;
        }

        // 1st enter: move to the next page for confirmation
        if ((currentPage == CODING_SAVE_SLOT)) {
            // Ask slot from user
            slotIndex = userSlotIndex();
            printf(
                "Slot %d selected, press Enter to confirm or 9 to go back.\n", slotIndex);
            // Change pageState to PAINTING_SAVE_CONFIRM
            currentPage = CODING_SAVE_CONFIRM;
        }
        else if (currentPage == CODING_LOAD_SLOT) {
            // Ask slot from user
            slotIndex = userSlotIndex();
            printf(
                "Slot %d selected, press Enter to confirm or 9 to go back.\n", slotIndex);
            // Change pageState to PAINTING_SAVE_CONFIRM
            currentPage = CODING_LOAD_CONFIRM;
        }
        // 2nd enter: confirm then go back to original savedColor canvas
        else if ((currentPage == CODING_SAVE_CONFIRM) ||
                 (currentPage == CODING_LOAD_CONFIRM)) {
            // Define local variable
            int val = -1;
            // Activate keyboard press input
            checkAllButtons();
            // Show the confirm page
            // Print new canvas for PAINTING_SAVE_CONFIRM or PAINTING_LOAD_CONFIRM once
            if (printOnce == 0) {
                // Erase old screen, make it all black
                fill_color(offColor);
                for (int arrayRow = 0; arrayRow < 8; arrayRow++) {
                    // array[0] = bottom = LED row 7, so flip the row
                    int ledRow = 7 - arrayRow;

                    for (int arrayCol = 0; arrayCol < 8; arrayCol++) {
                        // col[0] = rightmost = LED col 7, so flip the col
                        int ledCol = 7 - arrayCol;

                        // Get the saveConfirmLogo[slotIndex][7][7] or the 64th LED as
                        // starting point
                        int idx = ledRow * 8 + ledCol;

                        // Choose which screen to show for confirmation
                        switch (currentPage) {
                            case CODING_SAVE_CONFIRM:
                                val = saveConfirmLogo[slotIndex][arrayRow][arrayCol];
                                break;
                            case CODING_LOAD_CONFIRM:
                                val = loadConfirmLogo[slotIndex][arrayRow][arrayCol];
                                break;
                        }

                        // Print the cursor/selection indicator sits on this screen)
                        if (val == 1)
                            // set_color(idx, pointerColor);
                            setColorLEDScaled(idx, pointerColor, brightnessDivisor);
                        // Print the "go back" region of the icon
                        else if (val == 2)
                            // set_color(idx, returnColor);
                            setColorLEDScaled(idx, returnColor, brightnessDivisor);
                        // Print the "confirm" region of the icon
                        else if (val == 3)
                            // set_color(idx, confirmColor);
                            setColorLEDScaled(idx, confirmColor, brightnessDivisor);
                    }
                }
                // Print the new emulator screen for confirmation page
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

                printOnce = 1;
            }

            // Confirm
            // Enter = confirm save
            if (BTN_JUST_PRESSED(BTN_ENTER)) {
                // Save current screen for PAINTING_SAVE_CONFIRM page
                if (currentPage == CODING_SAVE_CONFIRM) {
                    for (int i = 0; i < NUM_LEDS; i++) {
                        codeSaveSlots[slotIndex][i] = wholeCodeCanvas[currentCanvas][i].currentColor;
                    }
                    printf("Saved painting to slot %d.\n", slotIndex);
                    currentPage = CODING_SPACE;
                }
                else if (currentPage == CODING_LOAD_CONFIRM) {
                    for (int i = 0; i < NUM_LEDS; i++) {
                        wholeCodeCanvas[currentCanvas][i].currentColor = codeSaveSlots[slotIndex][i];
                    }
                    printf("Load painting from slot %d.\n", slotIndex);
                    currentPage = CODING_SPACE;
                }

                slotChosen = true;
            }

            // Button 9 = back to slot select
            else if (BTN_JUST_PRESSED(BTN_9)) {
                printf("Going back to slot select.\n");
                slotIndex = -1;
                if ((currentPage == CODING_SAVE_CONFIRM)) {
                    currentPage = CODING_SAVE_SLOT;
                }
                else if (currentPage == CODING_LOAD_CONFIRM) {
                    currentPage = CODING_LOAD_SLOT;
                }
                // Allow to use the logic to go back for choosing slot
                printOnce = 0;
                returnToSlotPage = true;
            }
            // Update ALL button prev states AFTER logic — every tick
            updateAllButtons();
        }
    }
    // Draw canvas + pointer to the real one; Occur only when exiting
    if (slotChosen) {
        // Use custom function to print the 4 red led buttons and else
        renderCodingCanvas();
        buttonPressed = 0;
    }
    Delay_Ms(100);
    // Confirmation that all value is reset to default
    slotChosen = false;
    printOnce = 0;
}

// Helper functions: only be used in this C file

/**
 * @brief Show and print the `PAINTING_SAVE_SLOT` or `PAINTING_LOAD_SLOT` state
 **/
static inline void createSlotSelectionScreen(int row) {
    // Erase old screen, make it all black
    fill_color(offColor);
    // Draw the hardcoded S logo on rows 1-7
    if (currentPage == CODING_SAVE_SLOT) {
        for (int r = 0; r < 6; r++) {
            for (int c = 0; c < 8; c++) {
                int idx = r * 8 + c;
                if (saveLogo[r][c] == 1) {
                    // set_color(idx, scaledForeground);
                    setColorLEDScaled(idx, foregroundColor, brightnessDivisor);
                }
            }
        }
    }

    else if (currentPage == CODING_LOAD_SLOT) {
        for (int r = 0; r < 6; r++) {
            for (int c = 0; c < 8; c++) {
                int ledRow = 6 - r; // flip row (bottom-up)
                int ledCol = 7 - c; // flip col (right-to-left)
                int idx = ledRow * 8 + ledCol;
                if (loadLogo[r][c] == 1) {
                    // set_color(idx, scaledForeground);
                    setColorLEDScaled(idx, foregroundColor, brightnessDivisor);
                }
            }
        }
    }

    // Bottom row used for choossing slots visual
    for (int slot = 0; slot < 8; slot++) {
        int col = 7 - slot;
        int idx = row * 8 + col;

        // set_color(idx, slotColor);
        setColorLEDScaled(idx, slotColor, brightnessDivisor);
    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    printf("PRINTED EMULATOR \n");
}

/**
 * @brief Choose which slot user want to save/load to emulator
 **/
static inline int userSlotIndex() {
    int pressedNumber = -1; // store value here

    if (currentPage == CODING_SAVE_SLOT) {
        printf("Please choose which slot to save from 0 to 7 \n");
        printf("Please click button 1 (slot 0) to 8 (slot 7)\n");
    }
    else {
        printf("Please choose which slot to load from 0 to 7 \n");
        printf("Please click button 1 (slot 0) to 8 (slot 7)\n");
    }

    while (pressedNumber == -1) {
        // Check for input from 1 to 9
        checkNineButton();

        // Only do anything if at least one button was just pressed
        if (BTN_JUST_PRESSED(BTN_1)) {
            pressedNumber = 0;
        }
        else if (BTN_JUST_PRESSED(BTN_2)) {
            pressedNumber = 1;
        }
        else if (BTN_JUST_PRESSED(BTN_3)) {
            pressedNumber = 2;
        }
        else if (BTN_JUST_PRESSED(BTN_4)) {
            pressedNumber = 3;
        }
        else if (BTN_JUST_PRESSED(BTN_5)) {
            pressedNumber = 4;
        }
        else if (BTN_JUST_PRESSED(BTN_6)) {
            pressedNumber = 5;
        }
        else if (BTN_JUST_PRESSED(BTN_7)) {
            pressedNumber = 6;
        }
        else if (BTN_JUST_PRESSED(BTN_8)) {
            pressedNumber = 7;
        }

        // Update the previous data to check if button actually being press, just press or
        // release
        updateNineButton();
    }
    return pressedNumber;
}