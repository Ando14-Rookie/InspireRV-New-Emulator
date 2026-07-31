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

void playMelodyWithFlash(
    const uint8_t * notes, const uint16_t * durations, uint8_t len, color_t color) {
    // Clear the screen first
    clear();

    // Run the visual and audio
    for (uint8_t i = 0; i < len; i++) {
        // Fills Screen with Green/Red
        fill_color(color);

        // Prints the emulator screen
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

        // Plays note for its own duration (blocking or non-blocking, your driver's call)
        playEmuNote(notes[i], durations[i]);

        // Fills Screen with OFF LED between notes
        fill_color(offColor);

        // Print emulator screen
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

        // Brief gap so blinks look distinct, not one continuous glow
        Delay_Ms(50);
    }
}

/// @brief Play the correct visual
void flashCorrect(void) {
    // Used higher notes, so that the sound is clearer
    static const uint8_t notes[] = {NOTE_B5, NOTE_D6, NOTE_FS6, NOTE_B6};
    static const uint16_t durations[] = {240, 240, 240, 450};
    playMelodyWithFlash(notes, durations, 4, confirmColorCorrect);
}

/// @brief Play the wrong visual
void flashWrong(void) {
    // Used higher notes, so that the sound is clearer
    static const uint8_t notes[] = {NOTE_A5, NOTE_GS5};
    static const uint16_t durations[] = {400, 450};
    playMelodyWithFlash(notes, durations, 2, confirmColorWrong);
}
