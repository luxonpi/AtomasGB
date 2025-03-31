#include "game.h"
#include "sintables.h"

#include <stdlib.h> // For abs()


uint8_t highscore = 0;
uint8_t score = 0;

uint8_t numberOfAtoms = INITIAL_ATOMS;
uint8_t atom_values[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_angle[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_target_angle[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


uint8_t atom_radius[MAX_ATOMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

uint8_t cursor_position = 0;

uint8_t center_atom_value = 0;
uint8_t game_state = GAME_STATE_TITLE;


void init_game(void) {
    score = 0;
    highscore = 0;
    numberOfAtoms = INITIAL_ATOMS;

    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atom_values[i] = rand() % 3; 
        atom_radius[i] = 0;
    }

    update_atoms_angle(0, 0);

    // set angle to target angle
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atom_angle[i] = atom_target_angle[i];
    }

}

void spawn_center_atom(void) {

    if(rand() % 5 == 0) {
        center_atom_value = PLUS_ATOM;
    } else if(rand() % 20 == 1) {
        center_atom_value = MINUS_ATOM;
    } else {
        center_atom_value = rand() % 3;
    }

}
void insert_atom(uint8_t position, uint8_t value, uint8_t angle){

    int adjusted_position = (position + 1) % numberOfAtoms;
    
    // shift all atoms to the right
    for(int i = numberOfAtoms - 1; i > adjusted_position; i--) {
        atom_values[i] = atom_values[i - 1];
        atom_angle[i] = atom_angle[i - 1];
        atom_target_angle[i] = atom_target_angle[i - 1];
        atom_radius[i] = atom_radius[i - 1];
        
    }

    // insert the new atom
    atom_values[adjusted_position] = value;
    atom_radius[adjusted_position] = 0;
    atom_angle[adjusted_position] = angle;
    atom_target_angle[adjusted_position] = angle;

    numberOfAtoms++;

    update_atoms_angle(adjusted_position, angle);
    
    score += 1;
    
}

void update_game(){

    // increase radius of all atoms until they are 100
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atom_radius[i]+=10;
        if(atom_radius[i] > 100) {
            atom_radius[i] = 100;
        }
    }

    // animated atom angle, angles are from 0 to 99
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        int8_t diff = atom_target_angle[i] - atom_angle[i];
        
        // Adjust difference for shortest path around the circle
        if (diff > 50) {
            diff -= 100;
        } else if (diff < -50) {
            diff += 100;
        }

        // Move one step towards target
        if (diff > 0) {
            atom_angle[i] = (atom_angle[i] + 1) % 100;
        } else if (diff < 0) {
            atom_angle[i] = (atom_angle[i] + 99) % 100; // equivalent to subtracting 1
        }
    }


}

void update_atoms_angle(uint8_t position, uint8_t pivot_angle){
    // Calculate steps using the full range (100)
    uint16_t angleSteps = 10000 / numberOfAtoms;
    
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atom_target_angle[(i+position) % numberOfAtoms] = (pivot_angle + (angleSteps * i)/100 ) % 100;
    }
}

uint8_t get_cursor_angle(){

    uint16_t angleSteps = 10000 / numberOfAtoms;
    int adjusted_position = (cursor_position) % numberOfAtoms;
    return (atom_target_angle[adjusted_position]+angleSteps/200)%100;

}


#define SAVE_ADDR 0xA000  // Start address in SRAM

void save_highscore(uint8_t score) {

}

void load_highscore() {


   
}




