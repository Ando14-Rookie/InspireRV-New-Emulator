#pragma once
/**
 * @note Based on https://github.com/robsoncouto/arduino-songs
 */
// #define DEBUG_SOUND_PRINTF
#include "../ch32v003fun/ch32v003fun.h"

void JOY_sound(uint16_t freq, uint16_t dur);

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
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#define REST 0

/**
 * @brief Structure representing a range of notes in the melody.
 */
typedef struct noterange {
    int start; /** The starting index of the range (inclusive). */
    int end;   /** The ending index of the range (exclusive). */
} noterange_t;

/**
 * @brief Plays the music within the specified range of notes.
 * @param range The range of notes to be played.
 **/
void playMusic(noterange_t range);

/**
 * @brief Plays all the music in the melody.
 * 
 * This function calculates the number of notes in the 'melody' array and
 * calls the 'playMusic' function to play all the notes.
 */
void playAllMusic(void);

/**
 * @brief Array representing the melody notes and durations.
 *
 * The `melody` array stores the notes of the melody followed by their durations.
 * Each element in the array represents a note and its duration.
 * A positive number represents a regular note duration, while a negative number
 * represents a dotted note duration.
 *
 * Note durations are represented as follows:
 * - 4: quarter note
 * - 8: eighth note
 * - 16: sixteenth note
 *
 * For example, `NOTE_E5, 8` represents an eighth note of E5.
 *
 */
extern const int melody[];

extern const int notes;
// change this to make the song slower or faster
extern const int tempo;
// this calculates the duration of a whole note in ms
extern const int wholenote;

int convertDuration(int duration);

