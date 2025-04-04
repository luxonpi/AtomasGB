#include <stdlib.h> 
#include <rand.h>
#include <gb/gb.h> 

#include "game.h"
#include "sintables.h"
#include "display.h"
#include "sound.h"

uint8_t highscore = 0;
uint8_t score = 0;

uint8_t numberOfAtoms = INITIAL_ATOMS;
int8_t atom_values[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_angle[MAX_ATOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_target_angle[MAX_ATOMS] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t atom_radius[MAX_ATOMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
uint8_t atom_target_radius[MAX_ATOMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

uint8_t cursor_position = 0;
uint8_t moves_whithout_plus = 0;
uint8_t moves_whithout_minus = 0;
uint8_t moves_count = 0;
uint8_t highest_atom_number = 0;

uint8_t lowestValue = 1;
uint8_t valueRange = 3;

uint8_t center_atom_value = 0;
uint8_t game_state = GAME_STATE_TITLE;
uint8_t game_substate = GAME_SUBSTATE_INPUT;

// Add these variables at the top with other global variables
uint8_t atoms_to_middle_index = 0;
uint8_t atoms_to_middle_timer = 0;

// -1 means no reaction
int8_t reaction_pos=0;
int8_t maxReactionValue=-1;

// Add at the top with other global variables
uint8_t minus_absorb_position = 0;
uint8_t consecutive_reactions = 0;  // Track number of consecutive reactions

void start_new_game(void){

    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);

    reaction_pos=-1;
    game_substate = GAME_SUBSTATE_INPUT;
    consecutive_reactions = 0;  
    highest_atom_number = 0;

    moves_count = 0;
    moves_whithout_minus = 0;
    moves_whithout_plus = 0;
    lowestValue = 1;
    valueRange = 3;


    score = 0;
    highscore = 0;
    numberOfAtoms = INITIAL_ATOMS;

    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atom_values[i] = rand() % 3; 
        if(atom_values[i] > highest_atom_number && atom_values[i] < 118) highest_atom_number = atom_values[i];

        atom_radius[i] = 0;
        atom_target_radius[i] = 100;
    }

    update_atoms_angle(0, 0);

    // set angle to target angle
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atom_angle[i] = atom_target_angle[i];
    }

}

void end_move(void){

    moves_count++;
    moves_whithout_minus++;
    moves_whithout_plus++;

     if (moves_count % 30 == 0)
    {

        lowestValue++;

    }

    if ((moves_count + 250) % 300 == 0)
    {

        valueRange++;

    }

}

void absorb_atom(uint8_t position){

    minus_absorb_position = position;
    atom_target_radius[minus_absorb_position] = 0;
    game_substate = GAME_SUBSTATE_MINUS_ABSORB;
    play_absorb_atom_sound();

}

void spawn_center_atom(void) {

    moves_whithout_minus++;
    moves_whithout_plus++;

    uint8_t randomAtom = 0;
    if(numberOfAtoms>0) randomAtom = rand() % numberOfAtoms;

    if(rand() % 15 == 0 || moves_whithout_plus >= 5) {

        center_atom_value = PLUS_ATOM;
        moves_whithout_plus=0;

    } else if(moves_whithout_minus>=20) {

        center_atom_value = MINUS_ATOM;
        moves_whithout_minus=0;

    } else if(atom_values[randomAtom] < lowestValue && rand() % 2 == 0){

        center_atom_value = atom_values[randomAtom];

    }else {

        center_atom_value = rand() % valueRange + lowestValue;
   
    }

    if(center_atom_value > highest_atom_number && center_atom_value < 118) highest_atom_number = center_atom_value;
    

}


uint8_t check_reactions(void){

    if(numberOfAtoms<3) return 0;

    // check if any two atoms are touching
    for(uint8_t i = 0; i < numberOfAtoms; i++) {

        if (atom_values[i]==PLUS_ATOM || reaction_pos==i) {

            // Get left and right atom
            int8_t left_atom = (i-1);
            int8_t right_atom = (i+1) % numberOfAtoms;
            if (left_atom<0) left_atom = numberOfAtoms-1;

            if (atom_values[left_atom]==atom_values[right_atom] && atom_values[left_atom]!=PLUS_ATOM && atom_values[left_atom]!=MINUS_ATOM) {

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
        atom_target_radius[i] = atom_target_radius[i-1];
    }

    // insert the new atom
    atom_values[adjusted_position] = value;
    atom_radius[adjusted_position] = 0;
    atom_target_radius[adjusted_position] = 100;
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

    // Animate radius towards target
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        if(atom_radius[i] < atom_target_radius[i]) {
            atom_radius[i] += 10;
            if(atom_radius[i] > atom_target_radius[i]) {
                atom_radius[i] = atom_target_radius[i];
            }
        } else if(atom_radius[i] > atom_target_radius[i]) {
            atom_radius[i] -= 20;
            if(atom_radius[i] < atom_target_radius[i]) {
                atom_radius[i] = atom_target_radius[i];
            }
        }

        if(atom_radius[i] != atom_target_radius[i]) animation_done = 0;
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
            reaction_pos=-1;
            consecutive_reactions = 0;  // Reset counter when no more reactions
            maxReactionValue=-1;
            end_move();
        }

    }else if(game_substate == GAME_SUBSTATE_REACTION_ANIMATION && animation_done==1){

          // reaction

        if(maxReactionValue!=-1 && atom_values[reaction_pos] < maxReactionValue){
            atom_values[reaction_pos]= maxReactionValue;
        }

        atom_values[reaction_pos] += 1;
        score += atom_values[reaction_pos];

        if(atom_values[reaction_pos] > highest_atom_number && atom_values[reaction_pos] < 118) highest_atom_number = atom_values[reaction_pos];
        
        maxReactionValue = atom_values[reaction_pos];

        // Play merge sound with increasing pitch based on consecutive reactions
        consecutive_reactions++;
        play_merge_atom_sound(consecutive_reactions);
        
        // 1 2 3 3 3 4 4 4 2 
        // plus atom and right atom are removed, shift all atoms to the left by two
        for(uint8_t j = reaction_pos+3; j < numberOfAtoms; j++) {
            atom_values[j-2] = atom_values[j];
            atom_angle[j-2] = atom_angle[j];
            atom_target_angle[j-2] = atom_target_angle[j];
            atom_radius[j-2] = atom_radius[j];
            atom_target_radius[j-2] = atom_target_radius[j];
        }

        numberOfAtoms -= 2;

        update_atoms_angle(reaction_pos, atom_angle[reaction_pos]);
        
        game_substate = GAME_SUBSTATE_INSERT_ANIMATION;
        update_sprites();

    }else if(game_substate == GAME_SUBSTATE_MINUS_ABSORB && animation_done==1) {
        // Store the absorbed atom's value
        uint8_t absorbed_value = atom_values[minus_absorb_position];
        
        // Shift remaining atoms to the left
        for(uint8_t i = minus_absorb_position; i < numberOfAtoms - 1; i++) {
            atom_values[i] = atom_values[i + 1];
            atom_angle[i] = atom_angle[i + 1];
            atom_target_angle[i] = atom_target_angle[i + 1];
            atom_radius[i] = atom_radius[i + 1];
            atom_target_radius[i] = atom_target_radius[i + 1];
        }
        
        numberOfAtoms--;
        
        // Update angles for remaining atoms
        update_atoms_angle(minus_absorb_position, atom_angle[minus_absorb_position]);
        
        // Set center atom to absorbed value
        center_atom_value = absorbed_value;
        
        // Change to absorbed state where player can convert the atom
        game_substate = GAME_SUBSTATE_ATOM_ABSORBED;
        update_sprites();
        
    }else if(game_substate == GAME_SUBSTATE_ATOMS_TO_MIDDLE) {
        // Wait a bit between each atom
        if(atoms_to_middle_timer++ >= 2) {
            atoms_to_middle_timer = 0;
            
            if(atoms_to_middle_index < numberOfAtoms) {
                // Add current atom's value to score
                score += atom_values[atoms_to_middle_index];
                // Move atom to center by reducing target radius
                atom_target_radius[atoms_to_middle_index] = 0;
                atoms_to_middle_index++;
            } else if(animation_done) {
                // All atoms have reached center, transition to game over
                game_state = GAME_STATE_GAME_OVER;
                set_gameover_display();
                play_game_over_sound();
            }
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
    
    // If there's a minus atom in the middle, point directly to the selected atom
    if(center_atom_value == MINUS_ATOM) {
        return atom_target_angle[adjusted_position];
    }
    
    // Otherwise, position between atoms
    return (atom_target_angle[adjusted_position]+angleSteps/200)%100;
}


#define SAVE_ADDR 0xA000  // Start address in SRAM

void save_highscore(uint8_t score) {

}

void load_highscore() {


   
}




