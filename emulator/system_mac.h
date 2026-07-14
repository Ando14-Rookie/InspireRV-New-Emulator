#ifdef _APPLE_
    #ifndef SYSTEM_MAC_H
    #define SYSTEM_MAC_H

    // Will effect the CGKeyCode data type for physical key input for MacOS
    #include <ApplicationServices/ApplicationServices.h>
    #include <pthread.h>
    #include <stdbool.h>
    // Need the Delay_Ms declaration
    #include "./emulator_driver/emulator_driver.h"

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
    #define MAX_KEYS 6

    // Keep the actual definitions in exactly one .c file so only defined ONCE
    // thread: to handle multiple execution asynchronously

    // extern: to tell compiler variable will be instantsiated somewhere later
    extern pthread_t thread;

    // Act as a temporary container that tracks which keys are currently physically held down
    // at any given moment
    extern CGKeyCode pressedKeys[MAX_KEYS];

    // How many keys are in that snapshot
    extern int pressedKeyCount;

    extern bool invalidKey;

    // keyMutex: prepare a mutex (a lock that stops multiple threads from messing up shared
    // data)
    extern pthread_mutex_t keyMutex;

    // thread: unique identifier for the thread we will create to listen to keyboard events
    extern pthread_t thread;

    // Change from Windows Setup into MacOS Setup
    // CGKeyCode: 16-bit unsigned integer (virtual key code type on macOS)
    static const CGKeyCode ABCD[] = {A_Key, B_Key, C_Key, D_Key};
    static const CGKeyCode _0123456789ABCDEF[] = {_0_Key, _1_Key, _2_Key, _3_Key, _4_Key,
        _5_Key, _6_Key, _7_Key, _8_Key, _9_Key, A_Key, B_Key, C_Key, D_Key, E_Key, F_Key};

    void pthread_init();

    /**
     * @brief  Adds a key code to a list of currently pressed keys (ONLY WHEN 1st Time)
     * @param keyCode which key you are pressing
     *  */
    void addKey(CGKeyCode keyCode);

    /**
     * @brief Remove specific keyCode from list and then move left if it is in center
     * @param keyCode which key you are pressing
     **/
    void removeKey(CGKeyCode keyCode);

    #endif
#endif