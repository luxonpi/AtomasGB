#include "sound.h"
#include <gb/gb.h>
#include <stdint.h>

// Note frequencies
#define C4 256
#define D4 288
#define E4 320
#define F4 341
#define G4 384
#define A4 427
#define B4 480
#define C5 512
#define REST 0

#define song_length 40
uint8_t note_duration = 15;  // Slightly faster tempo
uint8_t note_timer = 0;
uint8_t current_note = 0;

// Mysterious melody pattern
const uint16_t melody[] = {
    C4, REST, G4, REST,   // Mysterious opening
    E4, REST, C4, REST,   // Echo effect
    B4, REST, G4, REST,   // Rising tension
    F4, REST, D4, REST,   // Falling pattern
    C4, C4, G4, G4,      // Repeating motif
    E4, E4, B4, REST,    // Building mystery
    G4, F4, E4, D4,      // Descending line
    C4, REST, C5, REST,   // Wide interval jump
    G4, REST, E4, REST,   // Fading out
    C4, REST, REST, REST  // Final note with silence
};

// Mysterious drum pattern (0 = no drum, 1 = drum hit)
const uint8_t drum_pattern[] = {
    1, 0, 0, 0,  // Sparse drum hits
    0, 0, 1, 0,  // for mysterious
    0, 1, 0, 0,  // atmosphere
    1, 0, 0, 0,  
    0, 0, 1, 0,
    0, 0, 0, 1,
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    1, 0, 0, 1
};


void init_sound(void) {
    // Turn on sound
    NR52_REG = 0x80;
    // Turn on all channels
    NR51_REG = 0xFF;
    // Set max volume for both channels
    NR50_REG = 0x77;
}




void start_music(void) {
    NR52_REG = 0x80;
    note_timer = 0;
    current_note = 0;
}


void update_background_music(void) {
    if (note_timer++ >= note_duration) {
        note_timer = 0;
        if (melody[current_note] != REST) {
            play_note(melody[current_note]);
        }
        if (drum_pattern[current_note]) {
            play_drum();
        }
        current_note = (current_note + 1) % song_length;
    }
} 

void stop_music(void) {
    NR52_REG = 0x00;
}




void play_note(uint16_t frequency) {
    if (frequency == REST) return;
    
    // Channel 1 (Square Wave)
    NR10_REG = 0x00;    // No sweep
    NR11_REG = 0x80;    // 50% duty cycle
    NR12_REG = 0x83;    // Volume = 8, fade down slower
    NR13_REG = (uint8_t)(2048 - (frequency));
    NR14_REG = 0x86;    // Frequency MSB + Initialize
}

void play_drum(void) {
    NR41_REG = 0x0F;    // Length
    NR42_REG = 0x71;    // Volume envelope
    NR43_REG = 0x52;    // Frequency & randomness
    NR44_REG = 0x80;    // Initialize
}

void play_transition_sound(void) {
    // Sweep from high to low with longer duration
    NR10_REG = 0x2E;    // Sweep down, longer time
    NR11_REG = 0x80;    // 50% duty cycle
    NR12_REG = 0xF7;    // Max volume, slower decay
    NR13_REG = 0x00;    // Start at higher frequency
    NR14_REG = 0x86;    // Trigger sound, higher octave
}

void play_add_atom_sound(void) {
    // Channel 4 (Noise) with quick high-pitched blip
    NR41_REG = 0x01;    // Very short length
    NR42_REG = 0x61;    // Lower volume (6), quick decay
    NR43_REG = 0x23;    // Mid-high frequency noise, less harsh
    NR44_REG = 0x80;    // Initialize
}

void play_merge_atom_sound(uint8_t merge_count) {
    // Play a note that increases in pitch with each merge
    uint16_t base_freq = C4;
    // Increase pitch by 12.5% for each merge (using integer arithmetic)
    uint16_t freq = base_freq - ((base_freq * merge_count) >> 3);
    play_note(freq);
}

void play_game_over_sound(void) {
    // Play a descending arpeggio for game over
    play_note(C5);
    delay(50);
    play_note(G4);
    delay(50);
    play_note(E4);
    delay(50);
    play_note(C4);
}

void play_absorb_atom_sound(void) {
    // Play a quick descending note for absorption
    NR10_REG = 0x00;    // No sweep
    NR11_REG = 0x80;    // 50% duty cycle
    NR12_REG = 0x83;    // Volume = 8, fade down slower
    NR13_REG = (uint8_t)(2048 - (C4 + 64));  // Start slightly higher than C4
    NR14_REG = 0x86;    // Frequency MSB + Initialize
}

void play_convert_atom_sound(void) {
    // Play a rising arpeggio for conversion
    play_note(C4);
    delay(30);
    play_note(E4);
    delay(30);
    play_note(G4);
}

