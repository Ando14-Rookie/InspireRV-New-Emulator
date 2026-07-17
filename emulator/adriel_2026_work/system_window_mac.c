#include "system_window_mac.h"
#include "extra_function.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ==========================================================
// 1. GLOBAL VARIABLE INSTANTIATION
// ==========================================================

// Keep the actual definitions in exactly one .c file so only defined ONCE
// thread: to handle multiple execution asynchronously 

// thread: unique identifier for the thread we will create to listen to keyboard events
pthread_t thread;

// Act as a temporary container that tracks which keys are currently physically held down at any given moment
KeyCode_t pressedKeys[MAX_KEYS] = {0};

// How many keys are in that snapshot
int pressedKeyCount = 0;

bool invalidKey = false;

// keyMutex: prepare a mutex (a lock that stops multiple threads from messing up shared data)
pthread_mutex_t keyMutex = PTHREAD_MUTEX_INITIALIZER;


// ==========================================================
// 2. COMMON UTILITY FUNCTIONS
// ==========================================================

void addKey(KeyCode_t keyCode) {
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode)
            return; // Key already in array
    }
    if (pressedKeyCount < MAX_KEYS) {
        pressedKeys[pressedKeyCount++] = keyCode;
    }
}

void removeKey(KeyCode_t keyCode) {
    for (int i = 0; i < pressedKeyCount; i++) {
        if (pressedKeys[i] == keyCode) {
            // Move last element to this position and decrease count
            pressedKeys[i] = pressedKeys[--pressedKeyCount];
            return;
        }
    }
}


// ==========================================================
// 3. MACOS-SPECIFIC IMPLEMENTATION
// ==========================================================
#ifdef __APPLE__

    // Only for MacOS

    /* low-level services to record, play, parse, convert, and
        synchronize audio streams*/
    // #include <AudioToolbox/AudioToolbox.h>

    /* provide low-level data types, plug-in support,
    XML property lists, etc
    */
    #include <CoreFoundation/CoreFoundation.h>

    // Store copy of "hook"
    static CFMachPortRef gEventTap = NULL;

    // Updates that list when macOS reports key down/up
    // Callback Function
    CGEventRef WindowProc(
        CGEventTapProxy proxy, CGEventType type, CGEventRef event, void * refcon) {
        (void)proxy;
        (void)refcon;

        // On macOS, if the tap gets disabled by a timeout or by user input,
        // re-enable it so the hook keeps working (equivalent to nCode < 0 passthrough)
        if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
            if (gEventTap) {
                printf("TAP DISABLED — reason: %d\n", type);
                CGEventTapEnable(gEventTap, true);
            }
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
        invalidKey =
            !((key >= _1_Key && key <= _9_Key) || (key >= A_Key && key <= F_Key) ||
                (key == I_Key || key == J_Key || key == K_Key || key == L_Key) ||
                (key == Enter_Key) || (key == C_Key) || (key == LeftCtrl_Key) ||
                (key == RightCtrl_Key));

        // printf("type=%d key=%d\n", type, key);

        // Filter: only allow character that don't cause keyboardLogic to be True
        if ((type == kCGEventKeyDown) && invalidKey) {
            // MessageBeep(MB_ICONERROR); // Optional: alert user (Windows-only, no direct
            // macOS equivalent used here)
            printf("Invalid selection 2.\n");
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
                // Allow mutex to be unlock so that when next hook come, it is not locked FOREVER
                pthread_mutex_unlock(&keyMutex);
                return NULL;
                // break;

            // Key is released (up)
            case kCGEventKeyUp:
                removeKey(key);
                // Allow mutex to be unlock so that when next hook come, it is not locked FOREVER
                pthread_mutex_unlock(&keyMutex);
                printf("pressedKeyCount=%d key=%d\n", pressedKeyCount, key);
                return NULL;
                // break;

            default:
                break;
        }

        // pthread_mutex_unlock(&keyMutex);

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

        // If hook is attached to keyboard successfully
        gEventTap = hook;

        // Create a run loop source and add it to the current run loop, then enable the tap
        CFRunLoopSourceRef runLoopSource =
            CFMachPortCreateRunLoopSource(kCFAllocatorDefault, hook, 0);
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
        CGEventTapEnable(hook, true);

        printf("Global keyboard Mac hook installed. Press 'Ctrl+C' in terminal to exit.\n");

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

#endif


// ==========================================================
// 4. WINDOWS-SPECIFIC IMPLEMENTATION
// ==========================================================
#ifdef _WIN32

    // Updates that list when macOS reports key down/up
    // Callback Function
    LRESULT CALLBACK WindowProc(int nCode, WPARAM wParam, LPARAM lParam) {
        // nCode: A code the hook procedure uses to determine how to process the message.
        // If nCode is less than zero, the hook procedure must pass the message to next chain.
        if (nCode < 0) {
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        // kbd is a real pointer to a KBDLLHOOKSTRUCT structure that contains information
        // lParam is like a number that may contain an address
        KBDLLHOOKSTRUCT * kbd = (KBDLLHOOKSTRUCT *)lParam;
        int key = kbd->vkCode;
        // Lock the mutex before modifying the pressedKeys array to ensure thread safety
        pthread_mutex_lock(&keyMutex);

        // Only when 1 to 9, C, I, J, K, L, Enter, Ctrl --> won't beep or stop executing
        // 1 to 9: button for coding and painting page; also to choosing LED matrix (2nd
        // input)
        // A-F: to choose LED matrix (1st input) I, J, K, L: to move between LED matrix
        invalidKey = !(
            ((key >= '1' && key <= '9') || (key >= VK_NUMPAD1 && key <= VK_NUMPAD9)) ||
            (key >= 'A' && key <= 'F') || (key >= 'I' && key <= 'L') || (key == VK_RETURN) ||
            (key == 'C') || (key == VK_LCONTROL) || (key == VK_RCONTROL));

        // printf("wParam=%lu key=%d\n", (unsigned long)wParam, key);

        // Filter: only allow character that don't cause keyboardLogic to be True
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && invalidKey) {
            MessageBeep(MB_ICONERROR); // Optional: alert user
            printf("Invalid selection 2.\n");
            // passes event or message information to the next hook procedure
            pthread_mutex_unlock(&keyMutex);
            // Handle none wanted key and block it from reaching the terminal
            return 1;

            // Sent valid key to Bash/VS Code terminal, e.g., i, j, k, l, etc
            // return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        // wParam: the identifier of the keyboard message (e.g., WM_KEYDOWN, WM_KEYUP, etc)
        switch (wParam) {
            // Key is pressed (down)
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:

                addKey(key);

                // Update to trigger changes to isRunning function
                currentKey = key;

                pthread_mutex_unlock(&keyMutex);
                return 1;
                // break;

            // Key is released (up)
            case WM_KEYUP:
            case WM_SYSKEYUP:
                removeKey(key);
                // pthread_mutex_unlock(&keyMutex);
                printf("pressedKeyCount=%d key=%d\n", pressedKeyCount, key);
                return 1;
                // break;
        }

        pthread_mutex_unlock(&keyMutex);

        // Ppasses event or message information to the next hook procedure
        //  Fallback path so the program keeps waiting for key press inputs
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

        printf("Global keyboard Windows hook installed. Press 'Ctrl+C' in terminal to exit.\n");

        // Message loop required to keep the hook active in the thread
        // retrieve messages from a thread's message queue and dispatch them to the
        // appropriate window procedures. This will keep the thread alive and allow it to
        // process keyboard events until the program is terminated.
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Unhook the event when the loop ends
        UnhookWindowsHookEx(hook);
        hook = NULL;
        return NULL;
    }

#endif


// ==========================================================
// 5. UNIFIED INITIALIZER
// ==========================================================

// Initialize the thread to listen to keyboard events
void pthread_init() { 
    pthread_create(&thread, NULL, eventTapThread, NULL); 
}