#ifdef _WIN32

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

    WORD pressedKeys[MAX_KEYS] = {};
    int pressedKeyCount = 0;
    pthread_t thread;
    pthread_mutex_t keyMutex = PTHREAD_MUTEX_INITIALIZER;

    void addKey(WORD keyCode) {
        for (int i = 0; i < pressedKeyCount; i++) {
            if (pressedKeys[i] == keyCode)
                return; // Key already in array
        }
        if (pressedKeyCount < MAX_KEYS) {
            pressedKeys[pressedKeyCount++] = keyCode;
        }
    }

    void removeKey(WORD keyCode) {
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
        bool invalidKey = !(
            ((key >= '1' && key <= '9') || (key >= VK_NUMPAD1 && key <= VK_NUMPAD9)) ||
            (key >= 'A' && key <= 'F') || (key >= 'I' && key <= 'L') || (key == VK_RETURN) ||
            (key == 'C') || (key == VK_LCONTROL) || (key == VK_RCONTROL));

        // printf("wParam=%lu key=%d\n", (unsigned long)wParam, key);

        // Filter: only allow character that don't cause keyboardLogic to be True
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && invalidKey) {
            MessageBeep(MB_ICONERROR); // Optional: alert user
            printf("Invalid selection 2.\5n");
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

#endif