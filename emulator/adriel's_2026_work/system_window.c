#include "system_window.h"
#include "extra_function.h"

// Only for MacOS

/* low-level services to record, play, parse, convert, and
    synchronize audio streams*/
// #include <AudioToolbox/AudioToolbox.h>

/* provide low-level data types, plug-in support,
XML property lists, etc
*/
// #include <CoreFoundation/CoreFoundation.h>
// For Windows

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_KEYS 6

WORD pressedKeys[MAX_KEYS];
int pressedKeyCount = 0;
// keyMutex: prepare a mutex (a lock that stops multiple threads from messing up shared
// data)
pthread_mutex_t keyMutex = PTHREAD_MUTEX_INITIALIZER;
// thread: unique identifier for the thread we will create to listen to keyboard events
pthread_t thread;
char pageState[] = "PAINTING_SPACE";  // actual definition lives here

// Adds a key code to a list of currently pressed keys (ONLY WHEN 1st Time)
void addKey(WORD keyCode) {
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode)
            return; // Key already in array
    }
    if (pressedKeyCount < MAX_KEYS) {
        pressedKeys[pressedKeyCount++] = keyCode;
    }
}

// Remove specific keyCode from list and then move left if it is in center
void removeKey(WORD keyCode) {
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode) {
            // Move last element to this position and decrease count
            pressedKeys[i] = pressedKeys[--pressedKeyCount];
            return;
        }
    }
}

bool is_key_pressed(WORD keyCode) {
    bool pressed = false;
    // Mutex (short for mutual exclusion) is like a bathroom door lock for your computer's
    // memory It ensures that only one thread can access the pressedKeys array at a time,
    // preventing conflicts and ensuring accurate key state tracking
    // lock mutex before accessing shared data
    pthread_mutex_lock(&keyMutex);
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode) {
            pressed = true;
            break;
        }
    }
    // unlock mutex after accessing shared data
    pthread_mutex_unlock(&keyMutex);
    return pressed;
}

// Updates that list when macOS reports key down/up
// Callback Function
LRESULT CALLBACK WindowProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // nCode: A code the hook procedure uses to determine how to process the message.
    // If nCode is less than zero, the hook procedure must pass the message to next chain.
    if (nCode < 0) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    // kbd is a rea l pointer to a KBDLLHOOKSTRUCT structure that contains information
    // lParam is like a number that may contain an address
    KBDLLHOOKSTRUCT * kbd = (KBDLLHOOKSTRUCT *)lParam;
    int key = kbd->vkCode;
    // Lock the mutex before modifying the pressedKeys array to ensure thread safety
    pthread_mutex_lock(&keyMutex);

    // Filter: only allow '1'-'9' (ASCII 0x31 - 0x39)
    if (key < _1_Key || key > _9_Key) {
        MessageBeep(MB_ICONERROR); // Optional: alert user
        // passes event or message information to the next hook procedure
        pthread_mutex_unlock(&keyMutex);
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    // wParam: the identifier of the keyboard message (e.g., WM_KEYDOWN, WM_KEYUP, etc)
    switch (wParam) {
        // Key is pressed (down)
        case WM_KEYDOWN:
            addKey(key);
            // Handle the painting and coding page for each button here
            handleButtonFunction(pageState, key);
            // Print the key code for debugging purposes
            // printf("Key code %u pressed\n", (unsigned int)key);
            break;

        // Key is released (up)
        case WM_KEYUP:
            removeKey(key);
            // printf("Key code %u released\n", (unsigned int)key);
            break;
    }
    pthread_mutex_unlock(&keyMutex);

    // passes event or message information to the next hook procedure
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Stay alive and keep waiting for keyboard events forever. Otherwise, thread 马上结束
void * eventTapThread(void * arg) {
    (void)arg;
    // install a hook procedure to monitor the system for certain types of events.
    HHOOK hook = SetWindowsHookEx(
        WH_KEYBOARD_LL, // requires the callback to be implemented in your own program.
        WindowProc,     // A pointer to the hook procedure
        GetModuleHandle(NULL), // to let Windows know which loaded program/module contains
                               // the hook code
        0); // which thread the hook belongs to. 0 means all threads in the same desktop.

    if (hook == NULL) {
        printf("Failed to install hook.\n");
        return NULL;
    }

    printf("Global keyboard hook installed. Press 'Ctrl+C' in terminal to exit.\n");

    // Message loop required to keep the hook active in the thread
    MSG msg;
    // retrieve messages from a thread's message queue and dispatch them to the
    // appropriate window procedures. This will keep the thread alive and allow it to
    // process keyboard events until the program is terminated.
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the event when the loop ends
    UnhookWindowsHookEx(hook);
    hook = NULL;

    return NULL;
}

// Initialize the thread to listen to keyboard events
void pthread_init() { pthread_create(&thread, NULL, eventTapThread, NULL); }

// int main(void) {
//     pthread_init();

//     while (1) {
//         // Prevent high usage of CPU by sleeping for a short duration before check again
//         Sleep(100);
//     }

//     return 0;
// }