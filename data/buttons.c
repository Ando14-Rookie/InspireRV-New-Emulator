#include "buttons.h"

static const int buttons[NUM_BUTTONS] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4,
    BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_10, BUTTON_11, BUTTON_12,
    BUTTON_13, BUTTON_14, BUTTON_15, BUTTON_16, BUTTON_17, BUTTON_18, BUTTON_19,
    BUTTON_20, BUTTON_21, BUTTON_22, BUTTON_23, BUTTON_24, BUTTON_25, BUTTON_26,
    BUTTON_27, BUTTON_28, BUTTON_29, BUTTON_30, BUTTON_31, BUTTON_32, BUTTON_33,
    BUTTON_34, BUTTON_35, BUTTON_36, BUTTON_37, BUTTON_38, BUTTON_39, BUTTON_40,
    BUTTON_41, BUTTON_42, BUTTON_43, BUTTON_44, BUTTON_45, BUTTON_46, BUTTON_47,
    BUTTON_48, BUTTON_49, BUTTON_50, BUTTON_51, BUTTON_52, BUTTON_53, BUTTON_54,
    BUTTON_55, BUTTON_56, BUTTON_57, BUTTON_58, BUTTON_59, BUTTON_60, BUTTON_61,
    BUTTON_62, BUTTON_63};

//Make all keyButton click equal to empty
ButtonState keyButtons[BTN_COUNT] = {};

// Call once per frame/loop to check one of the 9 buttons pressed or not
void checkNineButton(void) {
    keyButtons[BTN_1].cur   = JOY_first_pressed();
    keyButtons[BTN_2].cur   = JOY_second_pressed();
    keyButtons[BTN_3].cur   = JOY_third_pressed();
    keyButtons[BTN_4].cur   = JOY_fourth_pressed();
    keyButtons[BTN_5].cur   = JOY_fifth_pressed();
    keyButtons[BTN_6].cur   = JOY_sixth_pressed();
    keyButtons[BTN_7].cur   = JOY_seventh_pressed();
    keyButtons[BTN_8].cur   = JOY_eigth_pressed();
    keyButtons[BTN_9].cur   = JOY_ninth_pressed();
    keyButtons[BTN_UP].cur    = JOY_up_pressed();
    keyButtons[BTN_DOWN].cur  = JOY_down_pressed();
    keyButtons[BTN_LEFT].cur  = JOY_left_pressed();
    keyButtons[BTN_RIGHT].cur = JOY_right_pressed();
    keyButtons[BTN_ENTER].cur = JOY_enter_pressed();
}

// Call once per frame/loop to update 9 buttons only
void updateNineButton(void) {
    // prev update happens AFTER logic, or at end of update
    for (int i = 0; i <= BTN_9; i++) {
        keyButtons[i].prev = keyButtons[i].cur;
    }
}

// Call once per frame/loop to check one of the move/enter buttons pressed or not
void checkMoveButton(void) {
    keyButtons[BTN_UP].cur    = JOY_up_pressed();
    keyButtons[BTN_DOWN].cur  = JOY_down_pressed();
    keyButtons[BTN_LEFT].cur  = JOY_left_pressed();
    keyButtons[BTN_RIGHT].cur = JOY_right_pressed();
    keyButtons[BTN_ENTER].cur = JOY_enter_pressed();
}

// Call once per frame/loop to update I,J,K,L, Enter buttons only
void updateMoveButton(void) {
    // prev update happens AFTER logic, or at end of update
    for (int i = BTN_UP; i < BTN_COUNT; i++) {
        keyButtons[i].prev = keyButtons[i].cur;
    }
}

void checkAllButtons(void) {
    keyButtons[BTN_1].cur     = JOY_first_pressed();
    keyButtons[BTN_2].cur     = JOY_second_pressed();
    keyButtons[BTN_3].cur     = JOY_third_pressed();
    keyButtons[BTN_4].cur     = JOY_fourth_pressed();
    keyButtons[BTN_5].cur     = JOY_fifth_pressed();
    keyButtons[BTN_6].cur     = JOY_sixth_pressed();
    keyButtons[BTN_7].cur     = JOY_seventh_pressed();
    keyButtons[BTN_8].cur     = JOY_eigth_pressed();
    keyButtons[BTN_9].cur     = JOY_ninth_pressed();
    keyButtons[BTN_UP].cur    = JOY_up_pressed();
    keyButtons[BTN_DOWN].cur  = JOY_down_pressed();
    keyButtons[BTN_LEFT].cur  = JOY_left_pressed();
    keyButtons[BTN_RIGHT].cur = JOY_right_pressed();
    keyButtons[BTN_ENTER].cur = JOY_enter_pressed();
}

void updateAllButtons(void) {
    for (int i = 0; i < BTN_COUNT; i++) {
        keyButtons[i].prev = keyButtons[i].cur;
    }
}

/// @brief Only used in emulator for Window/MacOS
uint8_t ledCondition[NUM_BUTTONS] = {0}; //all set to 0 by default