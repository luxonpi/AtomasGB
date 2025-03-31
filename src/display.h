#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

// Tile IDs
#define TILE_ID_CURSOR 0
#define TILE_ID_PLUS 1
#define TILE_ID_MINUS 2
#define TILE_ID_HYDROGEN 3

// Screen dimensions
#define screenWidth 160 
#define screenHeight 144
#define screen_center_x 84
#define screen_center_y 80

// Define sprite structure
struct Sprite {
    int8_t y;
    int8_t x;
    uint8_t tile;
    uint8_t prop;
    
};

// Declare variables as external
extern struct Sprite AtomSprites[20];
extern struct Sprite MiddleAtom;
extern struct Sprite Cursor;
extern uint8_t frame_counter;

// Function declarations
void init_title_display(void);
void init_game_display(void);
void update_sprites(void);

#endif 