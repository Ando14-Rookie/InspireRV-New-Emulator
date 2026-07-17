# include "music.h"

uint16_t calculateEmuDuration(uint8_t emuDurVar){
    return (50+emuDurVar*100);
}

void playEmuNote(uint16_t frequency, uint16_t durationMs) {
    if (frequency == 0) {
        // Rest note: just delay without playing sound
        #if defined(_WIN32) || defined(_WIN64)
            Sleep(durationMs);
        #else
            usleep(durationMs * 1000);
        #endif
        return;
    }

    #if defined(_WIN32) || defined(_WIN64)
        // Windows Implementation
        Beep((DWORD)frequency, (DWORD)durationMs);

    #elif defined(__APPLE__)
        // macOS Implementation
        // 1. Setup our tone data configuration
        MacToneData tone = {
            .targetFrequency = (double)frequency,
            .sampleRate = 44100.0,
            .frameCounter = 0
        };

        // 2. Describe the audio component details
        AudioComponentDescription desc = {
            .componentType = kAudioUnitType_Output,
            .componentSubType = kAudioUnitSubType_DefaultOutput,
            .componentManufacturer = kAudioUnitManufacturer_Apple
        };

        AudioComponent comp = AudioComponentFindNext(NULL, &desc);
        AudioUnit toneUnit;
        AudioComponentInstanceNew(comp, &toneUnit);

        // 3. Attach our custom waveform renderer function
        AURenderCallbackStruct inputCallback = {
            .inputProc = ToneRenderCallback,
            .inputProcRefCon = &tone
        };
        AudioUnitSetProperty(toneUnit, kAudioUnitProperty_SetRenderCallback, 
                            kAudioUnitScope_Input, 0, &inputCallback, sizeof(inputCallback));

        // 4. Start playing the note
        AudioUnitInitialize(toneUnit);
        AudioOutputUnitStart(toneUnit);

        // 5. Let it play for the requested duration length
        usleep(durationMs * 1000);

        // 6. Tear down the audio engine component cleanly
        AudioOutputUnitStop(toneUnit);
        AudioUnitUninitialize(toneUnit);
        AudioComponentInstanceDispose(toneUnit);
    #endif
}

// const int melody[] = {

//   NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
//   NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
//   NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
//   NOTE_A4, 2, 
//   };

// const int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// // change this to make the song slower or faster
// const int tempo = 50;
// // this calculates the duration of a whole note in ms
// const int wholenote = (60000 * 4) / tempo;

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

// int convertDuration(int duration) {
//     int noteDuration = 0;
//     if (duration > 0) {
//         // regular note, just proceed
//         noteDuration = (wholenote) / duration;
//     }
//     else if (duration < 0) {
//         // dotted notes are represented with negative durations!!
//         noteDuration = (wholenote) / abs(duration);
//         noteDuration *= 1.5; // increases the duration in half for dotted notes
//     }
//     return noteDuration;
// }

// void playMusic(noterange_t range) {
//     // iterate over the notes of the melody.
//     // Remember, the array is twice the number of notes (notes + durations)
//     for (int thisNote = range.start * 2; thisNote < range.end * 2; thisNote += 2) {
//         JOY_sound(melody[thisNote], convertDuration(melody[thisNote + 1]));
//         Delay_Ms(10);
//     }
// }

// inline void playAllMusic(void) {
//     // sizeof gives the number of bytes, each int value is
//     // composed of two bytes (16 bits)
//     // there are two values per note (pitch and duration), so for each note
//     // there are four bytes
//     playMusic((noterange_t){0, notes});
// }