#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>


// https://gist.github.com/scazon/9bb7daab2d1a8342ade3
typedef enum {
  C3, Cd3, D3, Dd3, E3, F3, Fd3, G3, Gd3, A3, Ad3, B3,
  C4, Cd4, D4, Dd4, E4, F4, Fd4, G4, Gd4, A4, Ad4, B4,
  C5, Cd5, D5, Dd5, E5, F5, Fd5, G5, Gd5, A5, Ad5, B5,
  C6, Cd6, D6, Dd6, E6, F6, Fd6, G6, Gd6, A6, Ad6, B6,
  C7, Cd7, D7, Dd7, E7, F7, Fd7, G7, Gd7, A7, Ad7, B7,
  C8, Cd8, D8, Dd8, E8, F8, Fd8, G8, Gd8, A8, Ad8, B8,
  REST
} pitch;


typedef enum {
  START, COVERT, ABSORB, GAMEOVER, NEWELEMENT, ADD_ATOM
} sound;


void init_sound(void);

void start_music(void);
void update_background_music(void);
void stop_music(void);

void play_note(pitch *n);
void play_sound(sound s);
void play_merge_atom_sound(uint8_t merge_count);

#endif 