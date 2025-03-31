#ifndef SOUND_H
#define SOUND_H

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

extern const uint16_t melody[];
extern const uint8_t drum_pattern[];
extern uint8_t note_duration;
extern uint8_t note_timer;
extern uint8_t current_note;

void init_sound(void);
void play_note(uint16_t frequency);
void play_drum(void);
void play_transition_sound(void);
void play_add_atom_sound(void);
void update_background_music(void);

void stop_music(void);

#endif 