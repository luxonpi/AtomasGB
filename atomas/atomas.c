#include <gb/gb.h>
#include <stdint.h>

#include "spritetiles.h"
#include "sintables.h"

#define PLUS_ATOM 255
#define MINUS_ATOM 254

#define MAX_ATOMS 20
#define HIGHEST_ATOM_NUMBER 3

#define TILE_ID_CURSOR 0
#define TILE_ID_PLUS 1
#define TILE_ID_MINUS 2
#define TILE_ID_HYDROGEN 3

uint8_t screenWidth = 160; 
uint8_t screenHeight = 144;

uint8_t screen_center_x = 80+4;
uint8_t screen_center_y = 72+8;

// Define sprite structure
struct Sprite {
    int8_t y;
    int8_t x;
    uint8_t tile;
    uint8_t prop;
};

struct Sprite AtomSprites[20];
struct Sprite MiddleAtom;
struct Sprite Cursor;

uint8_t frame_counter = 0;  // For animation timing
const uint8_t MOVE_DELAY = 4;  // Move every 8 frames for slower animation
uint8_t angle = 0;  // Current angle for rotation
uint8_t angle_sub = 0;  // Sub-angle for smoother movement

uint8_t numberOfAtoms = 20; 

// Array of length 20 thats contains the value  for each atom  
// Plus atom 255 and minus is 254
uint8_t atom_values[MAX_ATOMS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
uint8_t cursor_position = 0;


void main(void){

    // Disable interrupts and turn off display
    disable_interrupts();
    DISPLAY_OFF;
    LCDC_REG = LCDCF_OFF | LCDCF_OBJ8 | LCDCF_OBJON | LCDCF_BGON;
  
    // Set sprite palette
    OBP0_REG = 0xE4;

    // Load Sprite Tiles into memory
    set_sprite_data(0, 7, SpriteTiles);

    // Initialize sprites in a circle
    for(uint8_t i = 0; i < numberOfAtoms; i++) {
        uint8_t sprite_angle = (i * 32) / 12;  // Distribute sprites evenly around the circle
        AtomSprites[i].x = screen_center_x + ((cos_table[sprite_angle]));
        AtomSprites[i].y = screen_center_x + ((sin_table[sprite_angle]));
        AtomSprites[i].tile = TILE_ID_HYDROGEN + (atom_values[i] % HIGHEST_ATOM_NUMBER);
        AtomSprites[i].prop = 0;
        
        // Set the sprite's tile and properties
        set_sprite_tile(i, AtomSprites[i].tile);
        set_sprite_prop(i, AtomSprites[i].prop);
    }

    MiddleAtom.x = screen_center_x;
    MiddleAtom.y = screen_center_y;
    MiddleAtom.tile = TILE_ID_PLUS;     
    MiddleAtom.prop = 0;
        
    // Set the sprite's tile and prope  rties
    set_sprite_tile(numberOfAtoms, MiddleAtom.tile);
    set_sprite_prop(numberOfAtoms, MiddleAtom.prop);

    Cursor.x = screen_center_x;
    Cursor.y = screen_center_y;
    Cursor.tile = TILE_ID_CURSOR;     
    Cursor.prop = 0;

    // Set the sprite's tile and properties
    set_sprite_tile(numberOfAtoms+1, Cursor.tile);
    set_sprite_prop(numberOfAtoms+1, Cursor.prop);

    // Turn display back on and enable interrupts
    DISPLAY_ON;
    enable_interrupts();

     // Store previous joypad state
    static uint8_t prev_joypad = 0;

    // Main loop
    while(1) {
        // Increment frame counter
        frame_counter++;
        
        // Rotate sprites every MOVE_DELAY frames
        if(frame_counter >= MOVE_DELAY) {
            frame_counter = 0;  // Reset counter
            
            // Increment sub-angle for smoother movement
            angle_sub++;
            if(angle_sub >= 2) {  // Only update main angle every 4 sub-steps
                angle_sub = 0;
                //angle = (angle + 1)%100 ;  // Increment angle (0-100)
            }
            
            // Update all sprite positions
            for(uint8_t i = 0; i < numberOfAtoms; i++) {

                uint8_t index = ( angle+(i*100)/numberOfAtoms) % 100;  // Calculate each sprite's angle

                AtomSprites[i].x = screen_center_x + ((cos_table[index]));
                AtomSprites[i].y = screen_center_y + ((sin_table[index]));
            }

        }

        // Set the cursor between two atoms based on the cursor position
        uint8_t index = ( angle+10+(cursor_position*100)/numberOfAtoms) % 100;  // Calculate each sprite's angle

        Cursor.x = screen_center_x + ((cos_table[index] * 4) / 5);
        Cursor.y = screen_center_y + ((sin_table[index] * 4) / 5);

        // Update the cursor position based on the button pressed
        if(joypad() & J_LEFT){
            cursor_position--;
        }
        if(joypad() & J_RIGHT){
            cursor_position++;
        }

       
        uint8_t curr_joypad = joypad();

        // Only trigger on button press down
        if((curr_joypad & J_A) && !(prev_joypad & J_A)){
            atom_values[cursor_position]++;
            AtomSprites[cursor_position].tile = TILE_ID_HYDROGEN + (atom_values[cursor_position] % HIGHEST_ATOM_NUMBER);

            // Update the sprite tile
            set_sprite_tile(cursor_position, AtomSprites[cursor_position].tile);
        }
        if((curr_joypad & J_B) && !(prev_joypad & J_B)){
            atom_values[cursor_position]--;
            AtomSprites[cursor_position].tile = TILE_ID_HYDROGEN + (atom_values[cursor_position] % HIGHEST_ATOM_NUMBER);

            // Update the sprite tile
            set_sprite_tile(cursor_position, AtomSprites[cursor_position].tile);
        }
        
        // Store current joypad state for next frame
        prev_joypad = curr_joypad;


        // Update sprite positions on screen
        for(uint8_t i = 0; i < numberOfAtoms; i++) {
            move_sprite(i, AtomSprites[i].x, AtomSprites[i].y);
        }

        move_sprite(numberOfAtoms, MiddleAtom.x, MiddleAtom.y);
        move_sprite(numberOfAtoms+1, Cursor.x, Cursor.y);

        // Wait for VBL
        vsync();
    }
}