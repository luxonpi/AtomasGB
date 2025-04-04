#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>  
#include <string.h> 

#include "display.h"
#include "titlescreen.h"
#include "spritetiles.h"
#include "game.h"
#include "sintables.h"
#include "gamebg.h"
#include "gameover.h"
#include "element_names.h"
// Place the background tiles into VRAM (set_bkg_data)  256 tiles
// Showing the background  (set_bkg_tiles)


// Sprite Variables
struct Sprite AtomSprites[20];
struct Sprite CenterAtom;
struct Sprite Cursor;

void show_titlescreen(void) {

    // Set Background
    set_bkg_data(0, sizeof(titlescreen_tiles) / 16, titlescreen_tiles);
    set_bkg_tiles(0, 0, 20, 18, (unsigned char *) titlescreen_map);

    // Hide Sprites
    for(uint8_t i = 0; i < MAX_ATOMS; i++) {
        move_sprite(i, 0, 0);
    }
    move_sprite(MAX_ATOMS, 0, 0);
    move_sprite(MAX_ATOMS+1, 0, 0);

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

uint8_t GetCharacterVRamTile(char character) {

    if(character >= '0'&&character <= '9') return (character-'0')+gamebg_TILE_COUNT;   
    if(character >= 'A'&&character <= 'Z') return (character-'A')+gamebg_TILE_COUNT+10;
    if(character >= 'a'&&character <= 'z') return (character-'a')+gamebg_TILE_COUNT+10;
    return 0;

}

void write_score(){

    char buffer[10];
    sprintf(buffer, "%d", score);  // Convert score to string
    int score_width = strlen(buffer);
    int total_width = score_width;
    int start_x = (screen_center_x - total_width) / 2; 

    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_bkg_xy_addr(start_x,1);
    
    // First, clear all tiles in the score area to white
    for(int i = 0; i < 10; i++) {
        set_vram_byte(vramAddress + i, 0xFF);  // Set to white tile
    }

    // Then write the score
    uint16_t index = 0;
    while(buffer[index] != '\0') {
        char character = buffer[index];
        uint8_t vramTile = GetCharacterVRamTile(character);
        set_vram_byte(vramAddress++, vramTile);
        index++;
    }


    // Write the highest atom name

    char* highest_atom_name = ElementNames[highest_atom_number%HIGHEST_ATOM_NUMBER];
    
    // Calculate text length
    int text_length = 0;
    while(highest_atom_name[text_length] != '\0') {
        text_length++;
    }
    
    // Calculate starting position to center the text
    start_x = (20 - text_length) / 2;  // 20 is the screen width in tiles
    
    vramAddress = get_bkg_xy_addr(start_x,1);
    
    // First, clear all tiles in the score area to white
    for(int i = 5; i < 15; i++) {  // Changed to 20 to clear full width
        set_vram_byte(get_bkg_xy_addr(i,1), 0xFF);  // Set to white tile
    }

    // Then write the score
    index = 0;
    while(highest_atom_name[index] != '\0') {
        char character = highest_atom_name[index];
        uint8_t vramTile = GetCharacterVRamTile(character);
        set_vram_byte(vramAddress++, vramTile);
        index++;
    }

}

void show_gamescreen(void) {
    
    // Set Background and font tiles
    fill_bkg_rect(0, 0, 20, 18, 0xFF);  // Clear entire background
    set_bkg_data(gamebg_TILE_COUNT,11+26,NumberTiles);
    set_bkg_data(0,gamebg_TILE_COUNT,gamebg_tiles);
    set_bkg_tiles(0,0,20,18,gamebg_map);

    // Write Score
    write_score();
  
    // Set Sprite Tiles
    set_sprite_data(0, SPRITE_TILE_COUNT, SpriteTiles);
    SHOW_SPRITES;
    
    // Set Sprite Properties
    CenterAtom.prop = 0;
    CenterAtom.x = screen_center_x;
    CenterAtom.y = screen_center_y;

    Cursor.x = screen_center_x;
    Cursor.y = screen_center_y;
    Cursor.tile = TILE_ID_CURSOR;     
    Cursor.prop = 0;

    set_sprite_prop(MAX_ATOMS, 0);
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

    CenterAtom.tile = getTileID(center_atom_value);     
    set_sprite_tile(MAX_ATOMS, CenterAtom.tile);
    
}
int min(int a, int b){
    return a < b ? a : b;
}

void set_gameover_display(void) {
        

       fill_bkg_rect(0, 0, 20, 18, 0xFF);  // Clear entire background


    set_bkg_data(gameover_TILE_COUNT,11,NumberTiles);
    set_bkg_data(0,gameover_TILE_COUNT,gameover_tiles);

    set_bkg_tiles(0,0,20,18,gameover_map);

   
    write_score();

       
    
        // Update sprite positions on screen
        for(uint8_t i = 0; i < 20; i++) {
            move_sprite(i, 0,0);
        }

        move_sprite(MAX_ATOMS, CenterAtom.x, CenterAtom.y);
        move_sprite(MAX_ATOMS+1, 0,0);


}

uint8_t blink_state = 0;
void update_game_display(void) {

        write_score();

        uint8_t cursor_angle = get_cursor_angle();  
        Cursor.x = screen_center_x +  cos_table[cursor_angle]*7/10;
        Cursor.y = screen_center_y +  sin_table[cursor_angle]*7/10;

          // Update all sprite positions
        for(uint8_t i = 0; i < numberOfAtoms; i++) {

            AtomSprites[i].x = screen_center_x + cos_table[atom_angle[i]]*atom_radius[i]/100;
            AtomSprites[i].y = screen_center_y + sin_table[atom_angle[i]]*atom_radius[i]/100;
            move_sprite(i, AtomSprites[i].x, AtomSprites[i].y);

            if(i==reaction_pos || (center_atom_value==MINUS_ATOM && i==cursor_position)){
                if (blink_state%3 == 0) {
                    set_sprite_prop(i, S_PRIORITY| S_PALETTE);  // Use OBP0 (default)
                } else {
                    set_sprite_prop(i, S_PRIORITY);  // Use OBP1
                }
                blink_state +=1;
                blink_state %= 100;
            } else {
                set_sprite_prop(i, S_PRIORITY);  // Use OBP1
            }
        }
    
        // Make cursor and center atom blink when an atom is absorbed and can be converted
        if(game_substate == GAME_SUBSTATE_ATOM_ABSORBED) {
            if (blink_state%2 == 0) {
                set_sprite_prop(MAX_ATOMS+1, S_PRIORITY| S_PALETTE);  // Use OBP0 (default)
                set_sprite_prop(MAX_ATOMS, S_PALETTE);    // Make center atom blink too
            } else {
                set_sprite_prop(MAX_ATOMS+1, S_PRIORITY);  // Use OBP1
                set_sprite_prop(MAX_ATOMS, 0);    // Use OBP1
            }
            blink_state +=1;
            blink_state %= 100;
        } else {
            set_sprite_prop(MAX_ATOMS+1, S_PRIORITY);  // Use OBP1
            set_sprite_prop(MAX_ATOMS, 0);    // Normal visibility
        }
      
        move_sprite(MAX_ATOMS, CenterAtom.x, CenterAtom.y);
        move_sprite(MAX_ATOMS+1, Cursor.x, Cursor.y);
}



