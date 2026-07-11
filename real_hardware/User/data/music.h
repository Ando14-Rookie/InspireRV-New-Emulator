#pragma once
/**
 * @note Based on https://github.com/robsoncouto/arduino-songs
 */
// #define DEBUG_SOUND_PRINTF
#include <ch32v003fun/ch32v003fun.h>

void JOY_sound(uint16_t freq, uint16_t dur) {
    int pin = PC3;
    funPinMode(pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    const int sysclk = 1000000;
    if (sysclk < freq)
        return;
    uint32_t delay_us = sysclk / 2 / freq;
    uint32_t dur_us = dur * 1000;
    while (dur_us > 1000) {
        if (freq)
            funDigitalWrite(pin, FUN_LOW);
        Delay_Us(delay_us);
        funDigitalWrite(pin, FUN_HIGH);
        Delay_Us(delay_us);
        dur_us -= 1000;
        if (dur_us > delay_us * 2)
            dur_us -= delay_us * 2;
    }
    if (freq)
        funDigitalWrite(pin, FUN_LOW);
    Delay_Us(delay_us);
    funDigitalWrite(pin, FUN_HIGH);
    Delay_Us(delay_us);
}

#define NOTE_C4  261.63
#define NOTE_CS4 277.18
#define NOTE_D4  293.66
#define NOTE_DS4 311.13
#define NOTE_E4  329.63
#define NOTE_F4  349.23
#define NOTE_FS4 369.99
#define NOTE_G4  392.00
#define NOTE_GS4 415.30
#define NOTE_A4  440.00
#define NOTE_AS4 466.16
#define NOTE_B4  493.88
#define NOTE_C5  523.25

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
 */
void playMusic(noterange_t range);

/**
 * @brief Plays all the music in the melody.
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
const int melody[] = {

  NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2, 
  };

const int notes = sizeof(melody) / sizeof(melody[0]) / 2;
// change this to make the song slower or faster
const int tempo = 50;
// this calculates the duration of a whole note in ms
const int wholenote = (60000 * 4) / tempo;

int convertDuration(int duration) {
    int noteDuration = 0;
    if (duration > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / duration;
    }
    else if (duration < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(duration);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
    }
    return noteDuration;
}

void playMusic(noterange_t range) {
    // iterate over the notes of the melody.
    // Remember, the array is twice the number of notes (notes + durations)
    for (int thisNote = range.start * 2; thisNote < range.end * 2; thisNote += 2) {
        JOY_sound(melody[thisNote], convertDuration(melody[thisNote + 1]));
        Delay_Ms(10);
    }
}

/**
 * @brief Plays all the music in the game.
 *
 * This function calculates the number of notes in the 'melody' array and
 * calls the 'playMusic' function to play all the notes.
 */
void playAllMusic(void) {
    // sizeof gives the number of bytes, each int value is
    // composed of two bytes (16 bits)
    // there are two values per note (pitch and duration), so for each note
    // there are four bytes
    playMusic((noterange_t){0, notes});
}
