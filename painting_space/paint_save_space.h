#ifndef PAINT_SAVE_SPACE_H
#define PAINT_SAVE_SPACE_H

#include "./painting_space.h"
#include "../emulator/adriel_2026_work/emulator_driver/emulator_driver.h"
#include "../emulator/adriel_2026_work/extra_function.h"

#define SAVE_SLOTS 8

// Store the 9 slots for LED 8x8; For ex, paintSaveSlots[0][NUM_LEDS] --> slot 1
extern color_t paintSaveSlots[SAVE_SLOTS][NUM_LEDS];

// Verify which slot has been used
// extern bool paintSlotUsed[SAVE_SLOTS];

/** 
 * @brief Initialize all 8 save slots and make each slot `paintSaveSlots` and `paintSlotUsed`
 * variable to false or empty by default
**/
void initPaintSaveSlots(void);

/** 
 * @brief Save a specific slot to `paintSaveSlots[slotIndex]` and ensure paintSlotUsed[slotIndex] 
 * has been used (true)
**/
void saveOrLoadPaintToSlot(void);

/** 
 * @brief Contains the logic that allows User to chose which slot to save. It also needs to have a 
 * `return/back` and `confirm` feature
**/
void renderSaveLoadSlotScreen(void);

/** 
 * @brief Contains the logic that allows User to chose which slot to load. It also needs to have a 
 * `return/back` and `confirm` feature
**/
void renderLoadSlotScreen(void);

#endif
