#ifndef GAME_H
#define GAME_H

#include <stdint.h>

// Game states
#define GAME_STATE_TITLE 0
#define GAME_STATE_GAME 1
#define GAME_STATE_COUNTDOWN 2
#define GAME_STATE_GAME_OVER 3

#define GAME_SUBSTATE_INPUT 0
#define GAME_SUBSTATE_ABSORB_ANIMATION 1
#define GAME_SUBSTATE_INSERT_ANIMATION 2
#define GAME_SUBSTATE_REACTION_ANIMATION 3
#define GAME_SUBSTATE_ATOMS_TO_MIDDLE 4
#define GAME_SUBSTATE_MINUS_ABSORB 5

// Atom configuration
#define INITIAL_ATOMS 5
#define MAX_ATOMS 20
#define HIGHEST_ATOM_NUMBER 3

// Special atom types
#define PLUS_ATOM 120
#define MINUS_ATOM 121

// External variables
extern uint8_t highscore;
extern uint8_t score;
extern uint8_t numberOfAtoms;

extern uint8_t atom_radius[MAX_ATOMS];
extern uint8_t atom_angle[MAX_ATOMS];
extern int8_t atom_values[MAX_ATOMS];

extern uint8_t cursor_position;
extern uint8_t center_atom_value;
extern uint8_t game_state;
extern uint8_t game_substate;
extern int8_t reaction_pos;

// Function declarations
void init_game(void);
void save_highscore(uint8_t score);
void load_highscore(void);
void spawn_center_atom(void);
void insert_atom(uint8_t position, uint8_t value, uint8_t angle);
void absorb_atom(uint8_t position);
uint8_t get_cursor_angle();
void update_atoms_angle(uint8_t position, uint8_t pivot_angle);
void update_game(void);

#endif /* GAME_H */ 