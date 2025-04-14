#ifndef DISPLAY_H
#define DISPLAY_H

// Screen dimensions
#define screenWidth 160 
#define screenHeight 144
#define screen_center_x 84
#define screen_center_y 92

// Define sprite structure
struct Sprite {
    int8_t y;
    int8_t x;
    uint8_t tile;
    uint8_t prop;
    
};

// Declare variables as external
extern struct Sprite AtomSprites[20];
extern struct Sprite CenterAtom;
extern struct Sprite Cursor;


// Function declarations
void show_titlescreen(void);
void show_gamescreen(void);
void show_highscore_screen(void);
void update_sprites(void);

#endif 