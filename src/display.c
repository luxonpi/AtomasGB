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

void clear_screen(void) {

    fill_bkg_rect(0, 0, 20, 18, 0xFF);
    set_bkg_data(gamebg_TILE_COUNT,11+27,FontTiles);

    // Clear sprites
    for(uint8_t i = 0; i < MAX_ATOMS + 2; i++) {
        move_sprite(i, 0, 0);
    }   

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
    if(character >= 'A'&&character <= 'Z') return (character-'A')+gamebg_TILE_COUNT+11;
    if(character >= 'a'&&character <= 'z') return (character-'a')+gamebg_TILE_COUNT+11;
    if(character == ' ') return gamebg_TILE_COUNT+10;
    return 0;

}


void show_gamescreen(void) {
    // Set Background and font tiles
    fill_bkg_rect(0, 0, 20, 18, 0xFF);  // Clear entire background
    set_bkg_data(gamebg_TILE_COUNT,11+26,FontTiles);
    set_bkg_data(0,gamebg_TILE_COUNT,gamebg_tiles);
    set_bkg_tiles(0,0,20,18,gamebg_map);

    // Set up sprite palettes
    OBP0_REG = 0xE4;  // Palette 0: White, Light Gray, Dark Gray, Black
    OBP1_REG = 0x1B;  // Palette 1: White, Light Gray, Dark Gray, Black (inverted)
  
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
    Cursor.prop = S_PRIORITY;  // Start with OBP1

    // Initialize all sprites with OBP1
    for(uint8_t i = 0; i < MAX_ATOMS + 2; i++) {
        set_sprite_prop(i, S_PRIORITY);
    }

    set_sprite_tile(MAX_ATOMS+1, Cursor.tile);

    update_sprites();
    update_game_display();
}

void update_sprites(void) {
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        AtomSprites[i].tile = getTileID(atoms[i].value);        
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

void print_text_vertically_centered(int y, char* text){

    int text_length = 0;
    while(text[text_length] != '\0') {
        text_length++;
    }

    int start_x = (20 - text_length) / 2;  // Center horizontally
    
    uint8_t* vramAddress = get_bkg_xy_addr(start_x, y);
    uint16_t index = 0;
    while(text[index] != '\0') {
        char character = text[index];
        uint8_t vramTile = GetCharacterVRamTile(character);
        set_vram_byte(vramAddress++, vramTile);
        index++;
    }
}

void show_highscore_screen(void) {
    // Clear entire background
    fill_bkg_rect(0, 0, 20, 18, 0xFF);
    set_bkg_data(gamebg_TILE_COUNT,11+27,FontTiles);


    // Hide all sprites
    for(uint8_t i = 0; i < MAX_ATOMS + 2; i++) {
        move_sprite(i, 0, 0);
    }
    
    // Write score and atom name centered vertically
    char score_buffer[10];
    sprintf(score_buffer, "%d", highscore);

    char* highest_atom_name = ElementNames[highelement%HIGHEST_ATOM_NUMBER];
    
    print_text_vertically_centered(6, "High score");    

    print_text_vertically_centered(8, score_buffer);    
    print_text_vertically_centered(10, highest_atom_name);
   
}



void set_gameover_display(void) {

    clear_screen();

    set_bkg_data(0, gameover_TILE_COUNT, gameover_tiles);
    set_bkg_data(gamebg_TILE_COUNT,38,FontTiles);
    set_bkg_tiles(0, 0, 20, 18, gameover_map);
    
    // Write score and atom name centered vertically
    char score_buffer[10];
    char highscore_buffer[20];
    char* latest_element_name = ElementNames[latest_element%HIGHEST_ATOM_NUMBER];

    sprintf(score_buffer, "%d", score);
    sprintf(highscore_buffer, "HS %d", highscore);

    if(new_highscore){
        strcpy(highscore_buffer, "New Highscore");

    }

    print_text_vertically_centered(15, highscore_buffer);    
    print_text_vertically_centered(11, score_buffer);    
    print_text_vertically_centered(9, latest_element_name);


}

uint8_t blink_state = 0;
void update_game_display(void) {

    // Clear the score and element text
    print_text_vertically_centered(1, "          ");
    print_text_vertically_centered(2, "          ");

    // Write Score
    char score_buffer[10];
    sprintf(score_buffer, "%d", score);
    print_text_vertically_centered(1, score_buffer);

    // Write latest element
    print_text_vertically_centered(2, ElementNames[latest_element%HIGHEST_ATOM_NUMBER]);

    uint8_t cursor_angle = get_cursor_angle();  
    Cursor.x = screen_center_x +  cos_table[cursor_angle]*7/10;
    Cursor.y = screen_center_y +  sin_table[cursor_angle]*7/10;

    // Update all sprite positions
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        AtomSprites[i].x = screen_center_x + cos_table[atoms[i].angle]*atoms[i].radius/100;
        AtomSprites[i].y = screen_center_y + sin_table[atoms[i].angle]*atoms[i].radius/100;
        move_sprite(i, AtomSprites[i].x, AtomSprites[i].y);

        // Reset sprite properties first
        set_sprite_prop(i, S_PRIORITY);  // Default to OBP1

        // Handle blinking for reaction or minus atom selection
        if(i == reaction_pos || (center_atom_value == MINUS_ATOM && i == cursor_position)) {
            if(blink_state % 3 == 0) {
                set_sprite_prop(i, S_PRIORITY | S_PALETTE);  // Use OBP0 for blinking
            }
        }
    }

    // Make cursor and center atom blink when an atom is absorbed
    if(game_state == GS_ATOM_ABSORBED) {
        if(blink_state % 2 == 0) {
            set_sprite_prop(MAX_ATOMS+1, S_PRIORITY | S_PALETTE);  // Cursor uses OBP0
            set_sprite_prop(MAX_ATOMS, S_PALETTE);  // Center atom uses OBP0
        } else {
            set_sprite_prop(MAX_ATOMS+1, S_PRIORITY);  // Cursor uses OBP1
            set_sprite_prop(MAX_ATOMS, 0);  // Center atom uses OBP1
        }
    } else {
        set_sprite_prop(MAX_ATOMS+1, S_PRIORITY);  // Cursor uses OBP1
        set_sprite_prop(MAX_ATOMS, 0);  // Center atom uses OBP1
    }

    // Increment blink state
    blink_state++;
    if(blink_state >= 100) blink_state = 0;
    
    move_sprite(MAX_ATOMS, CenterAtom.x, CenterAtom.y);
    move_sprite(MAX_ATOMS+1, Cursor.x, Cursor.y);
}



