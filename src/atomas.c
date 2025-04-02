#include <gb/gb.h>
#include <gbdk/font.h>
#include <gbdk/console.h> 
#include <stdint.h>
#include <stdio.h>  
#include <string.h>  
#include "sintables.h"
#include "sound.h"
#include "game.h"
#include "display.h"

void main(void){

    // Disable interrupts and turn off display
    disable_interrupts();
    DISPLAY_ON;
    SHOW_BKG;

    LCDC_REG = LCDCF_OFF | LCDCF_OBJ8 | LCDCF_OBJON | LCDCF_BGON;
    OBP0_REG = 0xE4;

    font_init();                  // Initialize the font system
    font_set(font_load(font_ibm)); // Load the built-in IBM font
    init_title_display();
    init_sound();

     // Store previous joypad state
    static uint8_t prev_joypad = 0;
      
    // ------------------ Main loop -------------------------------
    while(1) {

        uint8_t curr_joypad = joypad();

        if (game_state == GAME_STATE_TITLE) {

            // Update music
            update_background_music();

            // Start game with start button
            if ((curr_joypad & J_START) && !(prev_joypad & J_START)) {
            
                game_state = GAME_STATE_GAME;
                play_transition_sound();
                init_game();
                init_game_display();
                stop_music();
                init_sound();
                        
            }
   
        }else if (game_state == GAME_STATE_GAME) {

            // Select button to go to title screen
            if ((curr_joypad & J_SELECT) && !(prev_joypad & J_SELECT)) {
                game_state = GAME_STATE_TITLE;
                play_transition_sound();
                init_title_display();
                init_sound();
                current_note = 0;
                note_timer = 0;
            }

        }
    
        
        // Only process game logic if in game state
        if (game_state == GAME_STATE_GAME) {
           
            update_game();
            update_game_display();

            // if a is pressed, insert an atom
            if((curr_joypad & J_A) && !(prev_joypad & J_A) && game_substate == GAME_SUBSTATE_INPUT){
               
                play_add_atom_sound();
                insert_atom(cursor_position, center_atom_value, get_cursor_angle());
                spawn_center_atom();
                update_sprites();

                // Check if number of atomas is 20
                if(numberOfAtoms >= 20){
                    game_state = GAME_STATE_GAME_OVER;
                    set_gameover_display();
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
                init_title_display();
                init_sound();
                current_note = 0;
                note_timer = 0;
            }

        }
            

        prev_joypad = curr_joypad;
        vsync();
    }
}