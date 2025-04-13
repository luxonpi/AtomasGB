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

// Add these at the top of the file with other variables
#define MAX_PARTICLES 10
struct Particle {
    uint8_t x;
    uint8_t y;
    int8_t speed;
    uint8_t sprite_index;
};
struct Particle particles[MAX_PARTICLES];

int8_t get_noise(uint8_t x, uint8_t y, uint8_t time) {
    // Combine multiple sine waves with different frequencies for noise
    int8_t noise1 = sin_table[(x + time) % 256]/30;
    int8_t noise2 = sin_table[(y * 2 + time / 2) % 256]/30;
    int8_t noise3 = sin_table[(x * 3 + y + time / 4) % 256]/30;
    
    // Combine the noises and scale to a small range
    return (int8_t)((noise1 + noise2 + noise3) );
}

void show_titlescreen(void) {
    // Set Background
    set_bkg_data(0, sizeof(titlescreen_tiles) / 16, titlescreen_tiles);
    set_bkg_tiles(0, 0, 20, 18, (unsigned char *) titlescreen_map);

    // Load sprite data
    set_sprite_data(0, 5, SpriteTiles);
    SHOW_SPRITES;

    // Initialize particles
    for(uint8_t i = 0; i < MAX_PARTICLES; i++) {
       
        particles[i].speed = -1;  // Random upward speed
        particles[i].sprite_index = i;
        
        // Set particle sprite
        set_sprite_tile(i, TILE_ID_PARTICLE);
        set_sprite_prop(i, S_PRIORITY);  // Set priority to show above background
        move_sprite(i, particles[i].x, particles[i].y);
    }

    // Randomplay place all particles on screen without rand() function
    particles[0].x = 10;  
    particles[0].y = 10; 
    particles[1].x = 15;  
    particles[1].y = 15; 
    particles[2].x = 20;  
    particles[2].y = 40; 
    particles[3].x = 25;  
    particles[3].y = 80; 
    particles[4].x = 50;  
    particles[4].y = 30; 
    particles[5].x = 75;  
    particles[5].y = 135; 
    particles[6].x = 80;  
    particles[6].y = 40; 
    particles[7].x = 115;  
    particles[7].y = 3; 
    particles[8].x = 140;  
    particles[8].y = 50; 
    particles[9].x = 145;  
    particles[9].y = 55; 
    
    // Hide other sprites
    for(uint8_t i = MAX_PARTICLES; i < MAX_ATOMS + 2; i++) {
        move_sprite(i, 0, 0);
    }
}

void update_titlescreen_particles(void) {
    static uint8_t time = 0;
    time++;  // Increment time for animation
    
    for(uint8_t i = 0; i < MAX_PARTICLES; i++) {
        // Update particle position with noise-based movement
        particles[i].y += particles[i].speed;
        
        // Get noise value for this particle's position and time
        int8_t noise = get_noise(particles[i].x, particles[i].y, time);
        particles[i].x += noise;
        
        // Wrap around screen edges
        if(particles[i].x >= 160) particles[i].x = 0;
        if(particles[i].x < 0) particles[i].x = 159;
        
        // If particle reaches top, reset to bottom with slight horizontal variation
        if(particles[i].y < 0) {
            particles[i].y = 144;
            particles[i].x = (particles[i].x + 20) % 160;  // Move slightly right when resetting
        }
        
        // Update sprite position
        move_sprite(particles[i].sprite_index, particles[i].x, particles[i].y);
    }
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
    // Clear entire background
    fill_bkg_rect(0, 0, 20, 18, 0xFF);

    // Set up game over tiles and background
    set_bkg_data(gameover_TILE_COUNT, 11, NumberTiles);
    set_bkg_data(0, gameover_TILE_COUNT, gameover_tiles);
    set_bkg_tiles(0, 0, 20, 18, gameover_map);

    // Write score and atom name centered vertically
    char buffer[10];
    sprintf(buffer, "%d", score);
    int score_width = strlen(buffer);
    int start_x = (20 - score_width) / 2;  // Center horizontally
    
    // Write score at y=7 (vertically centered)
    uint8_t* vramAddress = get_bkg_xy_addr(start_x, 7);
    
    // Clear score area
    for(int i = 0; i < 10; i++) {
        set_vram_byte(get_bkg_xy_addr(i, 7), 0xFF);
    }
    
    // Write score
    uint16_t index = 0;
    while(buffer[index] != '\0') {
        char character = buffer[index];
        uint8_t vramTile = GetCharacterVRamTile(character);
        set_vram_byte(vramAddress++, vramTile);
        index++;
    }

    // Write highest atom name
    char* highest_atom_name = ElementNames[highest_atom_number%HIGHEST_ATOM_NUMBER];
    int text_length = 0;
    while(highest_atom_name[text_length] != '\0') {
        text_length++;
    }
    
    start_x = (20 - text_length) / 2;  // Center horizontally
    
    // Write atom name at y=8 (just below score)
    vramAddress = get_bkg_xy_addr(start_x, 8);
    
    // Clear atom name area
    for(int i = 0; i < 20; i++) {
        set_vram_byte(get_bkg_xy_addr(i, 8), 0xFF);
    }
    
    // Write atom name
    index = 0;
    while(highest_atom_name[index] != '\0') {
        char character = highest_atom_name[index];
        uint8_t vramTile = GetCharacterVRamTile(character);
        set_vram_byte(vramAddress++, vramTile);
        index++;
    }
    
    // Hide all sprites
    for(uint8_t i = 0; i < MAX_ATOMS + 2; i++) {
        move_sprite(i, 0, 0);
    }
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



