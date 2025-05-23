#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum {
    AS_TITLE,
    AS_GAME,
    AS_GAME_OVER,
    AS_HIGHSCORE_SCREEN
} app_state_t;

typedef enum {
    GS_INPUT,
    GS_ABSORB_ANIMATION,
    GS_INSERT_ANIMATION,
    GS_REACTION_ANIMATION,
    GS_ATOMS_TO_MIDDLE,
    GS_MINUS_ABSORB,
    GS_MINUS_ABSORB_ANIMATION,
    GS_ATOM_ABSORBED,
    GS_GAME_OVER
} game_state_t;

// Atom struct definition
typedef struct {
    int8_t value;
    uint8_t angle;
    uint8_t target_angle;
    uint8_t radius;
    uint8_t target_radius;
} Atom;


extern app_state_t app_state;
extern game_state_t game_state;

// Atom configuration
#define INITIAL_ATOMS 4
#define MAX_ATOMS 12

// Special atom types
#define PLUS_ATOM 120
#define MINUS_ATOM 121



// External variables

extern uint16_t score;
extern uint16_t latest_element;
extern uint8_t new_highscore;

extern uint8_t numberOfAtoms;
extern Atom atoms[MAX_ATOMS];

extern uint8_t cursor_position;
extern uint8_t center_atom_value;
extern int8_t reaction_pos;

// Function declarations
void start_new_game(void);
void save_highscore(uint8_t score);
void load_highscore(void);
void spawn_center_atom(void);
void insert_atom(uint8_t position, uint8_t value, uint8_t angle);
void absorb_atom(uint8_t position);
uint8_t get_cursor_angle();
void update_atoms_angle(uint8_t position, uint8_t pivot_angle);
void update_game(void);
uint8_t HasExistingSave(void);
void LoadSaveData(void);
void SaveData(void);

#endif /* GAME_H */ 