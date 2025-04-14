#include "sound.h"
#include <gb/gb.h>
#include <stdint.h>


/*

Sound Registers

Channel 1: Pulse Channel

NR10 - Sweep
NR11 - Duty Cycle
NR12 - Volume Envelope
NR13 - Frequency Low
NR14 - Frequency High

Channel 2: ?

NR21 - Duty Cycle
NR22 - Volume Envelope
NR23 - Frequency Low
NR24 - Frequency High

Channel 3: Wave Channel

NR30 - Sound on/off
NR31 - Length
NR32 - Volume Envelope
NR33 - Frequency Low
NR34 - Frequency High

Channel 4: Noise Channel

NR41 - Length/Waveform
NR42 - Volume Envelope
NR43 - LFSR Initialization
NR44 - Initialization



*/

// https://gist.github.com/scazon/9bb7daab2d1a8342ade3
const uint16_t frequencies[] = { //values based on a formula used by the GB processor
  44, 156, 262, 363, 457, 547, 631, 710, 786, 854, 923, 986,
  1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,
  1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,
  1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,
  1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,
  1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015,
  0
};

#define song_length 96
uint8_t note_duration = 3;  // Slightly faster tempo
uint8_t note_timer = 0;
uint8_t current_note = 0;

// Mysterious melody pattern
const pitch note_sequence[] = {
    // Section A
    C4, REST, REST, G5,REST,REST,C5,REST,
    C4, REST, REST, G5,REST,REST,C5,REST,
    C4, REST, REST, A5,REST,REST,E5,REST,  
    C4, REST, REST, F5,E5,F5,C5,REST,
    // Section A`
    C4, REST, REST, G5,REST,REST,C5,REST,
    C4, REST, REST, G5,REST,REST,C5,REST,
    C4, G5, REST, F5,REST,E5,C5,REST,  
    G5, E5, REST, C5,REST,REST,REST,REST,   
    // Section B
    C5, REST, G4, REST,E4,D4,C5,REST,
    C5, REST, G4, REST,E4,D4,C5,REST,
    C4, REST, REST, A5,REST,REST,E5,REST,  
    C5, REST, G4, REST,E4,D4,C4,REST,
    // Section B`
    

};

// Mysterious drum pattern (0 = no drum, 1 = drum hit)
const uint8_t drum_pattern[] = {
    1, 0, 0, 0, 1, 1, 0, 0, 
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 1, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 1, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 0,
    
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
        if (note_sequence[current_note] != REST) {
            play_note(note_sequence[current_note]);
        }
        if (drum_pattern[current_note]) {
            play_drum();
        }
        current_note = (current_note + 1) % song_length;
    }
} 

void stop_music(void) {

}


void play_note(pitch n) {
    if (n == REST) return;
    
        NR10_REG = 0x00;    // No sweep
        NR11_REG = 0x00;    // 12.5% duty cycle (more sine-like)
        NR12_REG = 0x83;    // Volume = 8, fade down slower
        NR13_REG = (UBYTE)frequencies[(n)]; //low bits of frequency
        NR14_REG = 0x80U | ((UWORD)frequencies[(n)]>>8); //high bits of frequency (and sound reset)

}

void play_sound(sound s){
    switch(s){
        case START:
            NR10_REG = 0x2E;    // Sweep down, longer time
            NR11_REG = 0x80;    // 50% duty cycle
            NR12_REG = 0xF7;    // Max volume, slower decay
            NR13_REG = 0x00;    // Start at higher frequency
            NR14_REG = 0x86;    // Trigger sound, higher octave
            break;
        case COVERT:
            play_note(C4);
            delay(30);
            play_note(E4);
            delay(30);
            play_note(G4);
            break;
        case ABSORB:
            NR10_REG = 0x1E;    // Sweep down, longer time
            NR11_REG = 0x80;    // 50% duty cycle
            NR12_REG = 0xF7;    // Max volume, slower decay
            NR13_REG = 0x00;    // Start at higher frequency
            NR14_REG = 0x86;    // Trigger sound, higher octave
            break;
        case GAMEOVER:
            play_note(C5);
            delay(50);
            play_note(G4);
            delay(50);
            play_note(E4);
            delay(50);
            // PLay noise 
        case ADD_ATOM:
            NR41_REG = 0x01;    // Very short length
            NR42_REG = 0x61;    // Lower volume (6), quick decay
            NR43_REG = 0x23;    // Mid-high frequency noise, less harsh
            NR44_REG = 0x80;    // Initialize
            break;
    }
}

void play_drum(void) {
    NR41_REG = 0x0F;    // Length
    NR42_REG = 0x71;    // Volume envelope
    NR43_REG = 0x52;    // Frequency & randomness
    NR44_REG = 0x80;    // Initialize
}


void play_merge_atom_sound(uint8_t merge_count) {
  
    play_note(C5+merge_count);

}





