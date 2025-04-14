#ifndef SAVED_DATA_HEADER
#define SAVED_DATA_HEADER

#include <gb/gb.h>

void save_game(uint16_t highscore, uint16_t highelement);
void load_save(uint16_t * highscore, uint16_t* highelement);

#endif