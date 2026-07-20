#include "music.h"

#pragma once
/**
 * @note Based on https://github.com/robsoncouto/arduino-songs
 */
// #define DEBUG_SOUND_PRINTF
#include "../ch32v003fun/ch32v003fun.h"

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

void playMusic(noterange_t range);

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
