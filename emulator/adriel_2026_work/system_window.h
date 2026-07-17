#ifdef _WIN32
    #ifndef SYSTEM_WINDOW_H
    #define SYSTEM_WINDOW_H

    // Will effect the CGKeyCode data type for physical key input for MacOS
    // #include <ApplicationServices/ApplicationServices.h>
    #include <pthread.h>
    #include <stdbool.h>
    // Use WindowsAPI to define the WORD data type
    #include <windows.h>
    // Need the Delay_Ms declaration
    #include "./emulator_driver/emulator_driver.h"

    void pthread_init(void);

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
    #define MAX_KEYS 6

    // Keep the actual definitions in exactly one .c file so only defined ONCE
    //thread: to handle multiple execution asynchronously 

    //extern: to tell compiler variable will be instantsiated somewhere later
    extern pthread_t thread;

    // Act as a temporary container that tracks which keys are currently physically held down at any given moment
    extern WORD pressedKeys[MAX_KEYS];

    // How many keys are in that snapshot
    extern int pressedKeyCount;

    extern bool invalidKey;

    // keyMutex: prepare a mutex (a lock that stops multiple threads from messing up shared data)
    extern pthread_mutex_t keyMutex;

    // thread: unique identifier for the thread we will create to listen to keyboard events
    extern pthread_t thread;

    // Change from MacOS into Windows Setup
    // WORD: 16-bit unsigned integer
    static const WORD ABCD[] = {A_Key, B_Key, C_Key, D_Key};
    static const WORD _0123456789ABCDEF[] = {_0_Key, _1_Key, _2_Key, _3_Key, _4_Key,
        _5_Key, _6_Key, _7_Key, _8_Key, _9_Key, A_Key, B_Key, C_Key, D_Key, E_Key, F_Key};

    /**
     * @brief  Adds a key code to a list of currently pressed keys (ONLY WHEN 1st Time)
     * @param keyCode which key you are pressing
     *  */
    void addKey(WORD keyCode);

    /** 
     * @brief Remove specific keyCode from list and then move left if it is in center
     * @param keyCode which key you are pressing
     **/
    void removeKey(WORD keyCode);

    #endif
#endif