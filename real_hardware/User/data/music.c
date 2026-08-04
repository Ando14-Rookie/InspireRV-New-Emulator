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

void playMelodyWithFlashHW(
    const uint16_t * notes, const uint8_t * durations, uint8_t len, color_t color) {
    // Clear the screen first
    clear();

    // Run the visual and audio
    for (uint8_t i = 0; i < len; i++) {
        // Fills Screen with Green/Red
        fill_color(color);

        // Prints the emulator screen
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

        // Plays note for its own duration (blocking or non-blocking, your driver's call)
        JOY_sound(notes[i], durations[i]);

        // Fills Screen with OFF LED between notes
        fill_color((color_t){0, 0, 0});

        // Prints the emulator screen again
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

        // Brief gap so blinks look distinct, not one continuous glow
        Delay_Ms(5);
    }
}

void flashCorrect(void) {
    static const uint16_t notes[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
    static const uint8_t durations[] = {150, 150, 150, 200};
    playMelodyWithFlashHW(notes, durations, 4, greenColor);
}

void flashWrong(void) {
    static const uint16_t notes[] = {NOTE_E4, NOTE_C4};
    static const uint8_t durations[] = {200, 250};
    playMelodyWithFlashHW(notes, durations, 2, redColor);
}

