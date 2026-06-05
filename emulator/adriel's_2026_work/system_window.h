#ifndef SYSTEM_MAC_H
#define SYSTEM_MAC_H

// Will effect the CGKeyCode data type for physical key input for MacOS
// #include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include <stdbool.h>
// Use WindowsAPI to define the WORD data type
#include <windows.h>

// Virtual-Key Codes: to identifykey input for Windows ONLY
#define A_Key 0x41
#define B_Key 0x42
#define C_Key 0x43
#define D_Key 0x44
#define E_Key 0x45
#define F_Key 0x46
#define I_Key 0x47
#define J_Key 0x48
#define K_Key 0x49
#define L_Key 0x4A
#define U_Key 0x4B
#define O_Key 0x4C
#define P_Key 0x4D
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

//thread: to handle multiple execution asynchronously 
pthread_t thread;

#define MAX_KEYS 6
WORD pressedKeys[MAX_KEYS];
int pressedKeyCount_;

// Change from MacOS into Windows Setup
// WORD: 16-bit unsigned integer
static const WORD ABCD[] = {A_Key, B_Key, C_Key, D_Key};
static const WORD _0123456789ABCDEF[] = {_0_Key, _1_Key, _2_Key, _3_Key, _4_Key,
    _5_Key, _6_Key, _7_Key, _8_Key, _9_Key, A_Key, B_Key, C_Key, D_Key, E_Key, F_Key};

void pthread_init();

bool is_key_pressed(WORD keyCode);

#endif