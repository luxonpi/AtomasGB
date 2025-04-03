#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

void init_sound(void);

void start_music(void);
void update_background_music(void);
void stop_music(void);

void play_note(uint16_t frequency);
void play_drum(void);
void play_transition_sound(void);
void play_add_atom_sound(void);
void play_merge_atom_sound(uint8_t merge_count);
void play_game_over_sound(void);
void play_absorb_atom_sound(void);
void play_convert_atom_sound(void);

#endif 