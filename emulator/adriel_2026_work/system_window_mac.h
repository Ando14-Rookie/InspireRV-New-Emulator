#ifndef SYSTEM_WINDOW_MAC_H
#define SYSTEM_WINDOW_MAC_H

#include <pthread.h>
#include <stdbool.h>

// ==========================================================
// 1. PLATFORM DETECTION & SPECIFIC TYPES
// ==========================================================

#ifdef __APPLE__
    // Will effect the CGKeyCode data type for physical key input for MacOS
    #include <ApplicationServices/ApplicationServices.h>
    
    // CGKeyCode: 16-bit unsigned integer (virtual key code type on macOS)
    typedef CGKeyCode KeyCode_t;

    // Virtual-Key Codes: to identify key input for MacOS ONLY
    // (macOS uses CGKeyCode values, defined in <Carbon/Carbon.h> / kVK_* constants)
    #define A_Key 0x00
    #define B_Key 0x0B
    #define C_Key 0x08
    #define D_Key 0x02
    #define E_Key 0x0E
    #define F_Key 0x03
    #define I_Key 0x22
    #define J_Key 0x26
    #define K_Key 0x28
    #define L_Key 0x25
    #define U_Key 0x20
    #define O_Key 0x1F
    #define P_Key 0x23
    #define _0_Key 0x1D
    #define _1_Key 0x12
    #define _2_Key 0x13
    #define _3_Key 0x14
    #define _4_Key 0x15
    #define _5_Key 0x17
    #define _6_Key 0x16
    #define _7_Key 0x1A
    #define _8_Key 0x1C
    #define _9_Key 0x19
    #define Enter_Key 0x24
    #define LeftCtrl_Key 0x3B
    #define RightCtrl_Key 0x3E

#elif defined(_WIN32)
    // Use WindowsAPI to define the WORD data type
    #include <windows.h>
    
    // WORD: 16-bit unsigned integer
    typedef WORD KeyCode_t;

    // Virtual-Key Codes: to identifykey input for Windows ONLY
    #define A_Key 0x41
    #define B_Key 0x42
    #define C_Key 0x43
    #define D_Key 0x44
    #define E_Key 0x45
    #define F_Key 0x46
    #define I_Key 0x49
    #define J_Key 0x4A
    #define K_Key 0x4B
    #define L_Key 0x4C
    #define U_Key 0x4D
    #define O_Key 0x4E
    #define P_Key 0x4F
    #define _0_Key 0x30
    #define _1_Key 0x31
    #define _2_Key 0x32
    #define _3_Key 0x33
    #define _4_Key 0x34
    #define _5_Key 0x35
    #define _6_Key 0x36
    #define _7_Key 0x37
    #define _8_Key 0x38
    #define _9_Key 0x39
    #define Enter_Key 0x0D
    #define LeftCtrl_Key 0xA2
    #define RightCtrl_Key 0xA3
#endif

// ==========================================================
// 2. STATIC LOOKUP ARRAYS
// ==========================================================

#ifdef __APPLE__
    // Change from Windows Setup into MacOS Setup
    static const KeyCode_t ABCD[] = {A_Key, B_Key, C_Key, D_Key};
    static const KeyCode_t _0123456789ABCDEF[] = {_0_Key, _1_Key, _2_Key, _3_Key, _4_Key,
        _5_Key, _6_Key, _7_Key, _8_Key, _9_Key, A_Key, B_Key, C_Key, D_Key, E_Key, F_Key};
#else
    // Change from MacOS into Windows Setup
    static const KeyCode_t ABCD[] = {A_Key, B_Key, C_Key, D_Key};
    static const KeyCode_t _0123456789ABCDEF[] = {_0_Key, _1_Key, _2_Key, _3_Key, _4_Key,
        _5_Key, _6_Key, _7_Key, _8_Key, _9_Key, A_Key, B_Key, C_Key, D_Key, E_Key, F_Key};
#endif

// ==========================================================
// 3. CROSS-PLATFORM EXTERNS & FUNCTIONS
// ==========================================================

// Need the Delay_Ms declaration
#include "./emulator_driver/emulator_driver.h"

#define MAX_KEYS 6

// Keep the actual definitions in exactly one .c file so only defined ONCE
// thread: to handle multiple execution asynchronously 

// extern: to tell compiler variable will be instantsiated somewhere later
// thread: unique identifier for the thread we will create to listen to keyboard events
extern pthread_t thread;

// Act as a temporary container that tracks which keys are currently physically held down at any given moment
extern KeyCode_t pressedKeys[MAX_KEYS];

// How many keys are in that snapshot
extern int pressedKeyCount;

extern bool invalidKey;

// keyMutex: prepare a mutex (a lock that stops multiple threads from messing up shared data)
extern pthread_mutex_t keyMutex;

void pthread_init(void);

/**
 * @brief  Adds a key code to a list of currently pressed keys (ONLY WHEN 1st Time)
 * @param keyCode which key you are pressing
 *  */
void addKey(KeyCode_t keyCode);

/** 
 * @brief Remove specific keyCode from list and then move left if it is in center
 * @param keyCode which key you are pressing
 **/
void removeKey(KeyCode_t keyCode);

#endif // SYSTEM_WINDOW_H