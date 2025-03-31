#include <gb/gb.h>
#include <gbdk/font.h>
#include <gbdk/console.h> 
#include <stdint.h>
#include <stdio.h>  
#include <string.h> 

#include "display.h"
#include "titlescreen.h"
#include "spritetiles.h"
#include "game.h"
#include "sintables.h"

// Define the global variables here
struct Sprite AtomSprites[20];
struct Sprite MiddleAtom;
struct Sprite Cursor;
uint8_t frame_counter = 0;

// Replace init_text_display with this simpler version
void write_score(uint8_t _score) {
    // Move score display initialization here
    char buffer[10];
    sprintf(buffer, "%d", _score);

    int score_width = strlen(buffer);
    int total_width = score_width;
    int start_x = (screen_center_x - total_width) / 2; 

    gotoxy(start_x, 1);  // Position at top of screen
    printf("%d", _score);
}

uint8_t getTileID(uint8_t value){

    if(value == PLUS_ATOM) {
        return TILE_ID_PLUS;
    } else if(value == MINUS_ATOM) {
        return TILE_ID_MINUS;
    } else {
        return TILE_ID_HYDROGEN + (value % HIGHEST_ATOM_NUMBER);
    }

}

void init_game_display(void) {
    // Clear the background first
    fill_bkg_rect(0, 0, 20, 18, 0x00);  // Clear entire background
    
    // Initialize font system again
    font_init();
    font_set(font_load(font_ibm));
    
    set_sprite_data(0, 7, SpriteTiles);
    BGP_REG = 0xE4;     // Set standard GB palette (dark to light)
    SHOW_SPRITES;
    SHOW_BKG;           // Make sure background is visible
    
    // Clear any existing text
    cls();              // Clear the screen before writing new text
    write_score(highscore);
    
    MiddleAtom.prop = 0;
    set_sprite_prop(MAX_ATOMS, 0);
    MiddleAtom.x = screen_center_x;
    MiddleAtom.y = screen_center_y;

    Cursor.x = screen_center_x;
    Cursor.y = screen_center_y;
    Cursor.tile = TILE_ID_CURSOR;     
    Cursor.prop = 0;

    // Set the sprite's tile and properties
    set_sprite_tile(MAX_ATOMS+1, Cursor.tile);
    set_sprite_prop(MAX_ATOMS+1, Cursor.prop);

    
    update_sprites();
    update_game_display();
}

void update_sprites(void) {

    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        AtomSprites[i].tile = getTileID(atom_values[i]);        
        set_sprite_tile(i, AtomSprites[i].tile);
    }

    // Hide sprites beyond initial number of atoms
    for(uint8_t i = numberOfAtoms; i < MAX_ATOMS; i++) {
        move_sprite(i, 0, 0);  // Move unused sprites off-screen
    }

    MiddleAtom.tile = getTileID(center_atom_value);     
    set_sprite_tile(MAX_ATOMS, MiddleAtom.tile);
    
}
int min(int a, int b){
    return a < b ? a : b;
}

void set_gameover_display(void) {
        
        cls();              // Clear the screen before writing new text

        gotoxy(screen_center_x/2-4, 1);  // Position at top of screen
        printf("GameOver");

        char buffer[10];
        sprintf(buffer, "%d", score);

        int score_width = strlen(buffer);
        int total_width = score_width;
        int start_x = (screen_center_x - total_width) / 2; 

        gotoxy(start_x, 2);  // Position at top of screen
        printf("%d", score);
    
        // Update sprite positions on screen
        for(uint8_t i = 0; i < 20; i++) {
            move_sprite(i, 0,0);
        }

        move_sprite(MAX_ATOMS, MiddleAtom.x, MiddleAtom.y);
        move_sprite(MAX_ATOMS+1, 0,0);


}

void update_game_display(void) {

        write_score(score);

        uint8_t cursor_angle = get_cursor_angle();  
        Cursor.x = screen_center_x +  cos_table[cursor_angle]*7/10;
        Cursor.y = screen_center_y +  sin_table[cursor_angle]*7/10;

          // Update all sprite positions
        for(uint8_t i = 0; i < numberOfAtoms; i++) {

            AtomSprites[i].x = screen_center_x + cos_table[atom_angle[i]]*atom_radius[i]/100;
            AtomSprites[i].y = screen_center_y + sin_table[atom_angle[i]]*atom_radius[i]/100;
            move_sprite(i, AtomSprites[i].x, AtomSprites[i].y);

        }
    
        // Update sprite positions on screen
        for(uint8_t i = 0; i < numberOfAtoms; i++) {
            move_sprite(i, AtomSprites[i].x, AtomSprites[i].y);
        }

        move_sprite(MAX_ATOMS, MiddleAtom.x, MiddleAtom.y);
        move_sprite(MAX_ATOMS+1, Cursor.x, Cursor.y);


}




void init_title_display(void) {

    set_bkg_data(0, sizeof(titlescreen_tiles) / 16, titlescreen_tiles);
    set_bkg_tiles(0, 0, 20, 18, (unsigned char *) titlescreen_map);

    // Hide sprites
    for(uint8_t i = 0; i < MAX_ATOMS; i++) {
        move_sprite(i, 0, 0);
    }

    move_sprite(MAX_ATOMS, 0, 0);
    move_sprite(MAX_ATOMS+1, 0, 0);

    // Show background
    SHOW_BKG;


}