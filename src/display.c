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
#include "gamebg.h"
// Place the background tiles into VRAM (set_bkg_data)  256 tiles
// Showing the background  (set_bkg_tiles)


// Define the global variables here
struct Sprite AtomSprites[20];
struct Sprite MiddleAtom;
struct Sprite Cursor;
uint8_t frame_counter = 2;


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
    return 0;

}

void write_score(){

    char buffer[10];
    sprintf(buffer, "%d", score);

    int score_width = strlen(buffer);
    int total_width = score_width;
    int start_x = (screen_center_x - total_width) / 2; 

  
    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_bkg_xy_addr(start_x,1);
    uint16_t index=0;

    while(buffer[index]!='\0'){

        char character = buffer[index];

        // Draw our character at the address
        // THEN, increment the address
        uint8_t vramTile = GetCharacterVRamTile(character);
        set_vram_byte(vramAddress++,vramTile);
        index++;
    }

}

void init_game_display(void) {
    
    // Set background to white


    fill_bkg_rect(0, 0, 20, 18, 0xFF);  // Clear entire background


    set_bkg_data(gamebg_TILE_COUNT,11,NumberTiles);
    set_bkg_data(0,gamebg_TILE_COUNT,gamebg_tiles);

    set_bkg_tiles(0,0,20,18,gamebg_map);


    write_score();
  
  
    set_sprite_data(0, 7, SpriteTiles);
    SHOW_SPRITES;
    
    // Clear any existing text
    
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

            // // only if plus or minus atom
            // if (atom_values[i]==PLUS_ATOM || atom_values[i]==MINUS_ATOM) {

            //     if (blink_state/5==5 || blink_state/5==10 ) {
            //         set_sprite_prop(i, S_PRIORITY| S_PALETTE);  // Use OBP0 (default)
            //     } else {
            //         set_sprite_prop(i, S_PRIORITY);  // Use OBP1
            //     }

            // blink_state +=1;
            // blink_state %= 200;
            // }


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

   


}