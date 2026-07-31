#pragma once
/**
 * @note Based on https://github.com/robsoncouto/arduino-songs
 */
// #define DEBUG_SOUND_PRINTF
// #include "ch32v003fun.h"

#include <stdint.h>
#include "../emulator/adriel_2026_work/system_window_mac.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <AudioUnit/AudioUnit.h>
    #include <unistd.h>

    // A simple container to pass tone properties to the macOS audio thread
    typedef struct {
        double targetFrequency;
        double sampleRate;
        uint32_t frameCounter;
    } MacToneData;

    // The rendering callback that generates the raw sound wave on macOS
    static OSStatus ToneRenderCallback(
        void *inRefCon, 
        AudioUnitRenderActionFlags *ioActionFlags, 
        const AudioTimeStamp *inTimeStamp, 
        UInt32 inBusNumber, 
        UInt32 inNumberFrames, 
        AudioBufferList *ioData) 
    {
        (void)ioActionFlags; (void)inTimeStamp; (void)inBusNumber;
        MacToneData *tone = (MacToneData *)inRefCon;
        Float32 *buffer = (Float32 *)ioData->mBuffers[0].mData;
        
        for (UInt32 i = 0; i < inNumberFrames; i++) {
            // Generate a basic square wave based on the frequency
            double period = tone->sampleRate / tone->targetFrequency;
            double halfPeriod = period / 2.0;
            double position = fmod((double)tone->frameCounter++, period);
            
            buffer[i] = (position < halfPeriod) ? 0.25f : -0.25f; // Volume level at 25%
        }
        return noErr;
    }
#endif

// void JOY_sound(uint16_t freq, uint16_t dur) {
//     int pin = PC3;
//     funPinMode(pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
//     const int sysclk = 1000000;
//     if (sysclk < freq)
//         return;
//     uint32_t delay_us = sysclk / 2 / freq;
//     uint32_t dur_us = dur * 1000;
//     while (dur_us > 1000) {
//         if (freq)
//             funDigitalWrite(pin, FUN_LOW);
//         Delay_Us(delay_us);
//         funDigitalWrite(pin, FUN_HIGH);
//         Delay_Us(delay_us);
//         dur_us -= 1000;
//         if (dur_us > delay_us * 2)
//             dur_us -= delay_us * 2;
//     }
//     if (freq)
//         funDigitalWrite(pin, FUN_LOW);
//     Delay_Us(delay_us);
//     funDigitalWrite(pin, FUN_HIGH);
//     Delay_Us(delay_us);
// }

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784

// The only playable with high quality when stored in uint8_t
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976

// #define NOTE_C7 2093
// #define NOTE_CS7 2217
// #define NOTE_D7 2349
// #define NOTE_DS7 2489
// #define NOTE_E7 2637
// #define NOTE_F7 2794
// #define NOTE_FS7 2960
// #define NOTE_G7 3136
// #define NOTE_GS7 3322
// #define NOTE_A7 3520
// #define NOTE_AS7 3729
// #define NOTE_B7 3951
// #define NOTE_C8 4186
// #define NOTE_CS8 4435
// #define NOTE_D8 4699
// #define NOTE_DS8 4978

#define REST 0

/** 
 * @brief Convert 3-bit emulator duration value (0~7) to milliseconds
 * @param emuDurVar How long will the note be played
**/
uint16_t calculateEmuDuration(uint8_t emuDurVar);

/** 
 * @brief Play one note at a given frequency for a given duration in ms
 * @param frequency Which note will be played
 * @param duration How long will the note be played
**/
void playEmuNote(uint16_t frequency, uint16_t durationMs);

/**
 * @brief Create short visual to show if user answer is correct or false
 * @param notes List of notes to use
 * @param duration How long should each note/blinking behaviour last
 * @param len How many times should it loops
 * @param color What color should be used for the blinking
 **/
void playMelodyWithFlash(
    const uint8_t * notes, const uint16_t * durations, uint8_t len, color_t color);

/// @brief Play the correct visual
void flashCorrect(void);

/// @brief Play the wrong visual
void flashWrong(void);