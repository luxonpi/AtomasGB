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

    // Initialize
    disable_interrupts();
    DISPLAY_ON;
    SHOW_BKG;
    LCDC_REG = LCDCF_OFF | LCDCF_OBJ8 | LCDCF_OBJON | LCDCF_BGON;
    OBP0_REG = 0xE4;
    font_init();                  // Initialize the font system

    show_titlescreen();
    init_sound();

    
     // Store previous joypad state
    static uint8_t prev_joypad = 0;
      
    // ------------------ Main loop ----------------------------// 
    while(1) {

        uint8_t curr_joypad = joypad();

        if (game_state == GAME_STATE_TITLE) {

            // Update music
            update_background_music();

            // Start game with start button
            if ((curr_joypad & J_START) && !(prev_joypad & J_START)) {
            
                game_state = GAME_STATE_GAME;
                play_transition_sound();
                start_new_game();
                show_gamescreen();
                stop_music();
               
                        
            }
   
        }else if (game_state == GAME_STATE_GAME) {

            // Select button to go to title screen
            if ((curr_joypad & J_SELECT) && !(prev_joypad & J_SELECT)) {
                game_state = GAME_STATE_TITLE;
                play_transition_sound();
                show_titlescreen();
                start_music();
            }

        }
    
        
        // Only process game logic if in game state
        if (game_state == GAME_STATE_GAME) {
           
            update_game();
            update_game_display();

            // if a is pressed, insert an atom
            if((curr_joypad & J_A) && !(prev_joypad & J_A) && game_substate == GAME_SUBSTATE_INPUT){
               
            
                if(center_atom_value == MINUS_ATOM) {
                    // Start minus atom absorption animation
                    play_add_atom_sound();
                    absorb_atom(cursor_position);
                    update_sprites();

                } else {

                    play_add_atom_sound();
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
                play_transition_sound();
                show_titlescreen();
                start_music();

            }

        }
            

        prev_joypad = curr_joypad;
        vsync();
    }


}