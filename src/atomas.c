#include <stdint.h>
#include <stdio.h>  
#include <string.h>  
#include <gb/gb.h>
#include <gbdk/font.h>

#include "sound.h"
#include "game.h"
#include "display.h"

void main(void){

    // Initialize //
    disable_interrupts();
    LCDC_REG = LCDCF_ON | LCDCF_OBJON | LCDCF_BGON;
    SPRITES_8x8;
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4;
    enable_interrupts();

    app_state = AS_TITLE;
    game_state = GS_INPUT;

    if(HasExistingSave()){
        LoadSaveData();
    }else{
        SaveData();
    }

    init_sound();
    show_titlescreen();
    
     // Store previous joypad state
    static uint8_t prev_joypad = 0;
      
    // ------------------ Main loop ----------------------------// 
    while(1) {

        uint8_t curr_joypad = joypad();

        if (app_state == AS_TITLE) {

            // Update music
            update_background_music();

            // Update particles
            update_titlescreen_particles();

            // Start game with start button
            if ((curr_joypad & J_START) && !(prev_joypad & J_START)) {
            
                app_state = AS_GAME;
                play_sound(START);
                start_new_game();
                show_gamescreen();
                stop_music();
                         
            }if ((curr_joypad & J_SELECT) && !(prev_joypad & J_SELECT)) {
            
                app_state = AS_HIGHSCORE_SCREEN;
                show_highscore_screen();

            }
   
        }else if (app_state == AS_GAME) {

            // Select button to go to title screen
            if ((curr_joypad & J_SELECT) && !(prev_joypad & J_SELECT)) {
                app_state = AS_TITLE;
                play_sound(START);
                show_titlescreen();
                start_music();
            }

        }else if (app_state == AS_HIGHSCORE_SCREEN) {

            update_background_music();

            // Select button to go to title screen
            if ((curr_joypad & J_SELECT) && !(prev_joypad & J_SELECT)) {
                app_state = AS_TITLE;
                show_titlescreen();
                
            }
        }
    
        // Only process game logic if in game state
        if (app_state == AS_GAME) {
           
            update_game();
            update_game_display();

            if(game_state == GS_GAME_OVER){
            
                app_state = AS_GAME_OVER;
                set_gameover_display();
                play_sound(GAMEOVER);
            
            }else if(game_state == GS_ATOM_ABSORBED && (curr_joypad & J_B) && !(prev_joypad & J_B)){
               
                // Convert the absorbed atom to a plus atom
                center_atom_value = PLUS_ATOM;
                play_sound(COVERT);
                game_state = GS_INPUT;
                update_sprites();
                
            }
            // if a is pressed, insert an atom
            else if((curr_joypad & J_A) && !(prev_joypad & J_A) && (game_state == GS_INPUT || game_state == GS_ATOM_ABSORBED)){
               
            
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


                

            }

            // Update the cursor position based on the button pressed
            if((curr_joypad & J_LEFT) && !(prev_joypad & J_LEFT)){
                cursor_position = (cursor_position - 1 + numberOfAtoms) % numberOfAtoms;
            }
            if((curr_joypad & J_RIGHT) && !(prev_joypad & J_RIGHT)){
                cursor_position = (cursor_position + 1) % numberOfAtoms;
            }

            
        }

        if(app_state == AS_GAME_OVER){

            if((curr_joypad & J_START) && !(prev_joypad & J_START)){
                app_state = AS_TITLE;
                play_sound(START);
                show_titlescreen();
                start_music();

            }

        }
            

        prev_joypad = curr_joypad;
        vsync();
    }


}