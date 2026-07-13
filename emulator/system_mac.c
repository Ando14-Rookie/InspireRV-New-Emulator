#include "system_mac.h"

#include "extra_function.h"

// Only for MacOS

/* low-level services to record, play, parse, convert, and
    synchronize audio streams*/
// #include <AudioToolbox/AudioToolbox.h>

/* provide low-level data types, plug-in support,
XML property lists, etc
*/
#include <CoreFoundation/CoreFoundation.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_KEYS 6

CGKeyCode pressedKeys[MAX_KEYS] = {};
int pressedKeyCount = 0;
pthread_t thread;
pthread_mutex_t keyMutex = PTHREAD_MUTEX_INITIALIZER;

void addKey(CGKeyCode keyCode) {
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode)
            return; // Key already in array
    }
    if (pressedKeyCount < MAX_KEYS) {
        pressedKeys[pressedKeyCount++] = keyCode;
    }
}

void removeKey(CGKeyCode keyCode) {
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode) {
            // Move last element to this position and decrease count
            pressedKeys[i] = pressedKeys[--pressedKeyCount];
            return;
        }
    }
}

// Updates that list when macOS reports key down/up
// Callback Function
CGEventRef WindowProc(
    CGEventTapProxy proxy, CGEventType type, CGEventRef event, void * refcon) {
    (void)proxy;
    (void)refcon;

    // On macOS, if the tap gets disabled by a timeout or by user input,
    // re-enable it so the hook keeps working (equivalent to nCode < 0 passthrough)
    if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
        return event;
    }

    // kbd is retrieved via CGEventGetIntegerValueField instead of a KBDLLHOOKSTRUCT
    // pointer CGKeyCode is the macOS equivalent of vkCode
    CGKeyCode key =
        (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    // Lock the mutex before modifying the pressedKeys array to ensure thread safety
    pthread_mutex_lock(&keyMutex);

    // Only when 1 to 9, C, I, J, K, L, Enter, Ctrl --> won't beep or stop executing
    // 1 to 9: button for coding and painting page; also to choosing LED matrix (2nd
    // input)
    // A-F: to choose LED matrix (1st input) I, J, K, L: to move between LED matrix
    bool invalidKey =
        !((key >= _1_Key && key <= _9_Key) || (key >= A_Key && key <= F_Key) ||
            (key == I_Key || key == J_Key || key == K_Key || key == L_Key) ||
            (key == Enter_Key) || (key == C_Key) || (key == LeftCtrl_Key) ||
            (key == RightCtrl_Key));

    // printf("type=%d key=%d\n", type, key);

    // Filter: only allow character that don't cause keyboardLogic to be True
    if ((type == kCGEventKeyDown) && invalidKey) {
        // MessageBeep(MB_ICONERROR); // Optional: alert user (Windows-only, no direct
        // macOS equivalent used here)
        printf("Invalid selection 2.\5n");
        // passes event or message information to the next hook procedure
        pthread_mutex_unlock(&keyMutex);
        // Handle none wanted key and block it from reaching the terminal
        return NULL;

        // Sent valid key to Bash/VS Code terminal, e.g., i, j, k, l, etc
        // return event;
    }

    // type: the identifier of the keyboard message (e.g., kCGEventKeyDown, kCGEventKeyUp,
    // etc)
    switch (type) {
        // Key is pressed (down)
        case kCGEventKeyDown:

            addKey(key);

            // Update to trigger changes to isRunning function
            currentKey = key;

            pthread_mutex_unlock(&keyMutex);
            return NULL;
            // break;

        // Key is released (up)
        case kCGEventKeyUp:
            removeKey(key);
            printf("pressedKeyCount=%d key=%d\n", pressedKeyCount, key);
            return NULL;
            // break;

        default:
            break;
    }

    pthread_mutex_unlock(&keyMutex);

    // Passes event or message information to the next hook procedure
    //  Fallback path so the program keeps waiting for key press inputs
    return event;
}

// Stay alive and keep waiting for keyboard events forever. Otherwise, thread 马上结束
void * eventTapThread(void * arg) {
    (void)arg;
    // install a hook procedure to monitor the system for certain types of events.
    CGEventMask eventMask =
        CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp);

    CFMachPortRef hook = CGEventTapCreate(
        kCGSessionEventTap,       // requires the callback to be implemented in your own
                                  // program.
        kCGHeadInsertEventTap,    // insert at head, similar priority intent to
                                  // WH_KEYBOARD_LL
        kCGEventTapOptionDefault, // allows modifying/blocking events (not listen-only)
        eventMask,
        WindowProc, // A pointer to the hook procedure
        NULL);      // refcon, unused here

    if (hook == NULL) {
        printf("Failed to install hook.\n");
        return NULL;
    }

    // Create a run loop source and add it to the current run loop, then enable the tap
    CFRunLoopSourceRef runLoopSource =
        CFMachPortCreateRunLoopSource(kCFAllocatorDefault, hook, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(hook, true);

    printf("Global keyboard hook installed. Press 'Ctrl+C' in terminal to exit.\n");

    // Message loop required to keep the hook active in the thread
    // retrieve messages from a thread's message queue and dispatch them to the
    // appropriate window procedures. This will keep the thread alive and allow it to
    // process keyboard events until the program is terminated.
    CFRunLoopRun();

    // Unhook the event when the loop ends
    CGEventTapEnable(hook, false);
    CFRelease(runLoopSource);
    CFRelease(hook);
    hook = NULL;
    return NULL;
}

// Initialize the thread to listen to keyboard events
void pthread_init() { pthread_create(&thread, NULL, eventTapThread, NULL); }