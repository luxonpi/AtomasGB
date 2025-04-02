#include "game.h"
#include "sintables.h"
#include <gb/gb.h> 
#include <stdlib.h> // For abs()
#include "display.h"
#include <rand.h>


uint8_t highscore = 0;
uint8_t score = 0;

uint8_t numberOfAtoms = INITIAL_ATOMS;
int8_t atom_values[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_angle[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_target_angle[MAX_ATOMS] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


uint8_t atom_radius[MAX_ATOMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

uint8_t cursor_position = 0;

uint8_t center_atom_value = 0;
uint8_t game_state = GAME_STATE_TITLE;
uint8_t game_substate = GAME_SUBSTATE_INPUT;


void init_game(void) {
    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);

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

int8_t reaction_pos=0;

uint8_t check_reactions(void){

    if(numberOfAtoms<3) return 0;

    // check if any two atoms are touching
    for(uint8_t i = 0; i < numberOfAtoms; i++) {

        if (atom_values[i]==PLUS_ATOM) {

            // Get left and right atom
            int8_t left_atom = (i-1);
            int8_t right_atom = (i+1) % numberOfAtoms;
            if (left_atom<0) left_atom = numberOfAtoms-1;

            if (atom_values[left_atom]==atom_values[right_atom]) {

                uint8_t plus_angle = atom_angle[i] ;

                reaction_pos = left_atom;

                atom_target_angle[left_atom] = plus_angle;
                atom_target_angle[right_atom] = plus_angle;
                // update target angle
                return 1;
        
            }

        }

    }
       
    return 0;

}


void insert_atom(uint8_t position, uint8_t value, uint8_t angle){

    int adjusted_position = (position + 1) % numberOfAtoms;
    
    // First shift atoms from the end to the insertion point
    for(int i = numberOfAtoms; i > adjusted_position; i--) {
        atom_values[i] = atom_values[i-1];
        atom_angle[i] = atom_angle[i-1];
        atom_target_angle[i] = atom_target_angle[i-1];
        atom_radius[i] = atom_radius[i-1];
    }

    // insert the new atom
    atom_values[adjusted_position] = value;
    atom_radius[adjusted_position] = 0;
    atom_angle[adjusted_position] = angle;
    atom_target_angle[adjusted_position] = angle;

    numberOfAtoms++;

    // Update angles for all atoms to ensure even distribution
    update_atoms_angle(adjusted_position, angle);
    
    score += 1;
    game_substate = GAME_SUBSTATE_INSERT_ANIMATION;
    update_sprites();
}

void update_game(){

    uint8_t animation_done = 1;

    // increase radius of all atoms until they are 100
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        
        atom_radius[i]+=10;
        if(atom_radius[i] > 100) {
            atom_radius[i] = 100;
        }

        if(atom_radius[i]!=100) animation_done = 0;
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

        if(atom_angle[i]!=atom_target_angle[i]) animation_done = 0;
    }

    if(game_substate == GAME_SUBSTATE_INSERT_ANIMATION && animation_done==1){

        int8_t hadreaction = check_reactions();

        if(hadreaction){
            game_substate = GAME_SUBSTATE_REACTION_ANIMATION;
        } else {
            game_substate = GAME_SUBSTATE_INPUT;
        }

    }else if(game_substate == GAME_SUBSTATE_REACTION_ANIMATION && animation_done==1){

          // reaction
        atom_values[reaction_pos] += 1;
        score += atom_values[reaction_pos];
        
        // 1 2 3 3 3 4 4 4 2 
        // plus atom and right atom are removed, shift all atoms to the left by two
        for(uint8_t j = reaction_pos+3; j < numberOfAtoms; j++) {
            atom_values[j-2] = atom_values[j];
            atom_angle[j-2] = atom_angle[j];
            atom_target_angle[j-2] = atom_target_angle[j];
            atom_radius[j-2] = atom_radius[j];
        }

        numberOfAtoms -= 2;

        update_atoms_angle(reaction_pos, atom_angle[reaction_pos]);
        
        game_substate = GAME_SUBSTATE_INSERT_ANIMATION;
        update_sprites();

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




