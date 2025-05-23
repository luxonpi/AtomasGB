#ifndef SAVED_DATA_HEADER
#define SAVED_DATA_HEADER

extern uint16_t highscore;
extern uint16_t highelement;

#include <gb/gb.h>

void save_game();
void load_save();

#endif