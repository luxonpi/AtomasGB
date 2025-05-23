#include <stdlib.h> 
#include <rand.h>
#include <gb/gb.h> 

#include "game.h"
#include "sintables.h"
#include "display.h"
#include "sound.h"
#include "savedata.h"


app_state_t app_state = AS_TITLE;
game_state_t game_state = GS_INPUT;

uint8_t numberOfAtoms = INITIAL_ATOMS;
Atom atoms[MAX_ATOMS] = {
    {0, 0, 1, 100, 100},  
    {0, 0, 0, 100, 100},  
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100},
    {0, 0, 0, 100, 100}
};


uint8_t new_highscore=0;

uint16_t score = 0;
uint16_t latest_element = 0;

uint8_t cursor_position = 0;
uint8_t moves_whithout_plus = 0;
uint8_t moves_whithout_minus = 0;
uint8_t moves_count = 0;

uint8_t lowestValue = 1;
uint8_t valueRange = 3;
uint8_t center_atom_value = 0;

// Add these variables at the top with other global variables
uint8_t atoms_to_middle_index = 0;
uint8_t atoms_to_middle_timer = 0;

// -1 means no reaction
int8_t reaction_pos=0;
int16_t maxReactionValue=-1;

// Add at the top with other global variables
uint8_t minus_absorb_position = 0;
uint8_t consecutive_reactions = 0;  // Track number of consecutive reactions

void start_new_game(void){

    // reset all atom values
    for(uint8_t i = 0; i < MAX_ATOMS; i++) {
        atoms[i].value = 0;
        atoms[i].angle = 0;
        atoms[i].target_angle = 1;
        atoms[i].radius = 100;
        atoms[i].target_radius = 100;
    }


    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);

    reaction_pos=-1;
    game_state = GS_INPUT;
    consecutive_reactions = 0;  
    latest_element = 0;

    moves_count = 0;
    moves_whithout_minus = 0;
    moves_whithout_plus = 0;
    lowestValue = 1;
    valueRange = 3;
    new_highscore=0;

    atoms_to_middle_index = 0;
    atoms_to_middle_timer = 0;

    score = 0;
    numberOfAtoms = INITIAL_ATOMS;

    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atoms[i].value = rand() % valueRange + lowestValue; 
        if(atoms[i].value > latest_element && atoms[i].value < 118) latest_element = atoms[i].value;

        atoms[i].radius = 0;
        atoms[i].target_radius = 100;
    }

    update_atoms_angle(0, 0);

    // set angle to target angle
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atoms[i].angle = atoms[i].target_angle;
    }

}

void end_move(void){

    reaction_pos=-1;
    consecutive_reactions = 0;  // Reset counter when no more reactions
    maxReactionValue=-1;

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
    atoms[minus_absorb_position].target_radius = 0;
    game_state = GS_MINUS_ABSORB;
    play_sound(ABSORB);

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

    } else if(atoms[randomAtom].value < lowestValue && rand() % 2 == 0){

        center_atom_value = atoms[randomAtom].value;

    }else {

        center_atom_value = rand() % valueRange + lowestValue;
   
    }

    if(center_atom_value > latest_element && center_atom_value < 118) latest_element = center_atom_value;
    

}


uint8_t check_reactions(void){

    if(numberOfAtoms<3) return 0;

    // check if any two atoms are touching
    for(uint8_t i = 0; i < numberOfAtoms; i++) {

        if (atoms[i].value==PLUS_ATOM || reaction_pos==i) {

            // Get left and right atom
            int8_t left_atom = (i-1);
            int8_t right_atom = (i+1) % numberOfAtoms;
            if (left_atom<0) left_atom = numberOfAtoms-1;

            if (atoms[left_atom].value==atoms[right_atom].value && atoms[left_atom].value!=PLUS_ATOM && atoms[left_atom].value!=MINUS_ATOM) {

                uint8_t plus_angle = atoms[i].angle;

                reaction_pos = left_atom;

                atoms[left_atom].target_angle = plus_angle;
                atoms[right_atom].target_angle = plus_angle;

                if(atoms[left_atom].value > maxReactionValue)
                maxReactionValue = atoms[left_atom].value;

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
        atoms[i] = atoms[i-1];
    }

    // insert the new atom
    atoms[adjusted_position].value = value;
    atoms[adjusted_position].radius = 0;
    atoms[adjusted_position].target_radius = 100;
    atoms[adjusted_position].angle = angle;
    atoms[adjusted_position].target_angle = angle;

    numberOfAtoms++;

    // Update angles for all atoms to ensure even distribution
    update_atoms_angle(adjusted_position, angle);
    
    score += 1;
    game_state = GS_INSERT_ANIMATION;
    update_sprites();
}

void update_game(){

    uint8_t animation_done = 1;

    // Animate radius towards target
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        if(atoms[i].radius < atoms[i].target_radius) {
            atoms[i].radius += 10;
            if(atoms[i].radius > atoms[i].target_radius) {
                atoms[i].radius = atoms[i].target_radius;
            }
        } else if(atoms[i].radius > atoms[i].target_radius) {
            atoms[i].radius -= 20;
            if(atoms[i].radius < atoms[i].target_radius) {
                atoms[i].radius = atoms[i].target_radius;
            }
        }

        if(atoms[i].radius != atoms[i].target_radius) animation_done = 0;
    }

    // animated atom angle, angles are from 0 to 99
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        int8_t diff = atoms[i].target_angle - atoms[i].angle;
        
        // Adjust difference for shortest path around the circle
        if (diff > 50) {
            diff -= 100;
        } else if (diff < -50) {
            diff += 100;
        }

        // Move one step towards target
        if (diff > 0) {
            atoms[i].angle = (atoms[i].angle + 1) % 100;
        } else if (diff < 0) {
            atoms[i].angle = (atoms[i].angle + 99) % 100; // equivalent to subtracting 1
        }

        if(atoms[i].angle != atoms[i].target_angle) animation_done = 0;
    }

    if(game_state == GS_INSERT_ANIMATION && animation_done==1){

        int8_t hadreaction = check_reactions();

        if(hadreaction){
            
            game_state = GS_REACTION_ANIMATION;

        }else if(numberOfAtoms>=MAX_ATOMS){

            game_state = GS_ATOMS_TO_MIDDLE;

        }else {

            game_state = GS_INPUT;
            end_move();

        }

    }else if(game_state == GS_REACTION_ANIMATION && animation_done==1){

        // reaction
        if(maxReactionValue!=-1 && atoms[reaction_pos].value < maxReactionValue){
            atoms[reaction_pos].value = maxReactionValue;
        }

        atoms[reaction_pos].value += 1;
        maxReactionValue +=1;
        score += atoms[reaction_pos].value;

        if(atoms[reaction_pos].value > latest_element && atoms[reaction_pos].value < 118) latest_element = atoms[reaction_pos].value;
       

        // Play merge sound with increasing pitch based on consecutive reactions
        consecutive_reactions++;
        play_merge_atom_sound(consecutive_reactions);

       
        
        // 1 2 3 3 3 4 4 4 2 
        // plus atom and right atom are removed, shift all atoms to the left by two
        for(uint8_t j = reaction_pos+3; j < numberOfAtoms; j++) {
            atoms[j-2] = atoms[j];
        }

        // Special case: plus atom is the last atom in the array
        // -> shift all atoms to the left by one
        if(reaction_pos+2 >= numberOfAtoms){
            for(uint8_t j = 0; j < numberOfAtoms-2; j++) {
                atoms[j] = atoms[j+1];
            }
            if(reaction_pos>0)
            reaction_pos--;
        }

        numberOfAtoms -= 2;

        update_atoms_angle(reaction_pos, atoms[reaction_pos].angle);
        
        game_state = GS_INSERT_ANIMATION;
        update_sprites();

    }else if(game_state == GS_MINUS_ABSORB && animation_done==1) {
        // Store the absorbed atom's value
        uint8_t absorbed_value = atoms[minus_absorb_position].value;
        
        // Shift remaining atoms to the left
        for(uint8_t i = minus_absorb_position; i < numberOfAtoms - 1; i++) {
            atoms[i] = atoms[i + 1];
        }
        
        numberOfAtoms--;
        
        // Update angles for remaining atoms
        update_atoms_angle(minus_absorb_position, atoms[minus_absorb_position].angle);
        
        // Set center atom to absorbed value
        center_atom_value = absorbed_value;
        
        // Change to absorbed state where player can convert the atom
        game_state = GS_ATOM_ABSORBED;
        update_sprites();
        
    }else if(game_state == GS_ATOMS_TO_MIDDLE) {
        // Wait a bit between each atom
        if(atoms_to_middle_timer++ >= 2) {
            atoms_to_middle_timer = 0;
            
            if(atoms_to_middle_index < numberOfAtoms) {
                // Add current atom's value to score
                
                if(atoms[atoms_to_middle_index].value!=PLUS_ATOM && atoms[atoms_to_middle_index].value!=MINUS_ATOM){
                    score += atoms[atoms_to_middle_index].value;
                    // Play a rising note based on the atom's value
                    play_note(C4 + (atoms_to_middle_index % 12));
                }
                // Move atom to center by reducing target radius
                atoms[atoms_to_middle_index].target_radius = 0;
                atoms_to_middle_index++;

            } else if(animation_done) {
                // All atoms have reached center, transition to game over
                game_state = GS_GAME_OVER;

                if(score > highscore){
                    highscore = score;
                    new_highscore=1;
                    save_game();
                    
                }

                if(latest_element > highelement){
                    highelement = latest_element;
                    save_game();

                }

             
            }
        }
    }

   
    
}

void update_atoms_angle(uint8_t position, uint8_t pivot_angle){
    // Calculate steps using the full range (100)
    uint16_t angleSteps = 10000 / numberOfAtoms;
    
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        atoms[(i+position) % numberOfAtoms].target_angle = (pivot_angle + (angleSteps * i)/100 ) % 100;
    }
}

uint8_t get_cursor_angle(){

    uint16_t angleSteps = 10000 / numberOfAtoms;
    int adjusted_position = (cursor_position) % numberOfAtoms;
    
    // If there's a minus atom in the middle, point directly to the selected atom
    if(center_atom_value == MINUS_ATOM) {
        return atoms[adjusted_position].target_angle;
    }
    
    // Otherwise, position between atoms
    return (atoms[adjusted_position].target_angle+angleSteps/200)%100;
}






