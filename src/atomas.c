#include <stdint.h>
#include <stdio.h>  
#include <string.h>  
#include <gb/gb.h>
#include <gbdk/font.h>

#include "sintables.h"
#include "sound.h"
#include "game.h"
#include "display.h"

void main(void){

    // Initialize //
    disable_interrupts();
    LCDC_REG = LCDCF_ON | LCDCF_OBJON | LCDCF_BGON;
    SPRITES_8x8;

    OBP0_REG = 0xE4;
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4;
    enable_interrupts();

    init_sound();
    show_titlescreen();
    
     // Store previous joypad state
    static uint8_t prev_joypad = 0;
      
    // ------------------ Main loop ----------------------------// 
    while(1) {

        uint8_t curr_joypad = joypad();

        if (game_state == GAME_STATE_TITLE) {

            // Update music
            update_background_music();

            // Update particles
            update_titlescreen_particles();

            // Start game with start button
            if ((curr_joypad & J_START) && !(prev_joypad & J_START)) {
            
                game_state = GAME_STATE_GAME;
                play_sound(START);
                start_new_game();
                show_gamescreen();
                stop_music();
               
                        
            }
   
        }else if (game_state == GAME_STATE_GAME) {

            // Select button to go to title screen
            if ((curr_joypad & J_SELECT) && !(prev_joypad & J_SELECT)) {
                game_state = GAME_STATE_TITLE;
                play_sound(START);
                show_titlescreen();
                start_music();
            }

        }
    
        
        // Only process game logic if in game state
        if (game_state == GAME_STATE_GAME) {
           
            update_game();
            update_game_display();

            if(game_substate == GAME_SUBSTATE_ATOM_ABSORBED && (curr_joypad & J_B) && !(prev_joypad & J_B)){
               
                // Convert the absorbed atom to a plus atom
                center_atom_value = PLUS_ATOM;
                play_sound(COVERT);
                game_substate = GAME_SUBSTATE_INPUT;
                update_sprites();
                
            }
            // if a is pressed, insert an atom
            else if((curr_joypad & J_A) && !(prev_joypad & J_A) && (game_substate == GAME_SUBSTATE_INPUT || game_substate == GAME_SUBSTATE_ATOM_ABSORBED)){
               
            
                if(center_atom_value == MINUS_ATOM) {
                    // Start minus atom absorption animation
                    play_sound(ABSORB);
                    absorb_atom(cursor_position);
                    update_sprites();

                } else {

                    play_sound(ADD_ATOM);
                    insert_atom(cursor_position, center_atom_value, get_cursor_angle());
                    spawn_center_atom();
                    update_sprites();
                }


                // Check if number of atoms is 20
                if(numberOfAtoms >= 20){
                    game_substate = GAME_SUBSTATE_ATOMS_TO_MIDDLE;
                    
                }

            }

            // Update the cursor position based on the button pressed
            if((curr_joypad & J_LEFT) && !(prev_joypad & J_LEFT)){
                cursor_position = (cursor_position - 1 + numberOfAtoms) % numberOfAtoms;
            }
            if((curr_joypad & J_RIGHT) && !(prev_joypad & J_RIGHT)){
                cursor_position = (cursor_position + 1) % numberOfAtoms;
            }

            
        }

        if(game_state == GAME_STATE_GAME_OVER){

            if((curr_joypad & J_START) && !(prev_joypad & J_START)){
                game_state = GAME_STATE_TITLE;
                play_sound(START);
                show_titlescreen();
                start_music();

            }

        }
            

        prev_joypad = curr_joypad;
        vsync();
    }


}