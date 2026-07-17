#ifndef CODE_SAVE_SPACE_H
#define CODE_SAVE_SPACE_H

#include "./coding_space.h"
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
#include "../emulator/adriel_2026_work/extra_function.h"

#define SAVE_SLOTS 8

// Store the 9 slots for LED 8x8; For ex, paintSaveSlots[0][NUM_LEDS] --> slot 1
extern color_t codeSaveSlots[SAVE_SLOTS][NUM_LEDS];

// Verify which slot has been used
// extern bool paintSlotUsed[SAVE_SLOTS];

/** UNUSED
 * @brief Initialize all 8 save slots and make each slot `codeSaveSlots` and `codeSlotUsed`
 * variable to false or empty by default
**/
// void initCodeSaveSlots(void);

/** 
 * @brief Save a specific slot to `codeSaveSlots[slotIndex]` and ensure codeSlotUsed[slotIndex] 
 * has been used (true)
**/
void saveOrLoadCodeToSlot(void);

/** 
 * @brief Contains the logic that allows User to chose which slot to save. It also needs to have a 
 * `return/back` and `confirm` feature
**/
void renderCodeSaveLoadSlotScreen(void);

/** 
 * @brief Contains the logic that allows User to chose which slot to load. It also needs to have a 
 * `return/back` and `confirm` feature
**/
void renderLoadSlotScreen(void);

#endif
