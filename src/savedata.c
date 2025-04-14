#include <gb/gb.h>
#include "savedata.h"

char *saved = (char *)0xa000; // Pointer to memory address
uint16_t *f_highscore = (uint16_t *)0xa001;
uint16_t *f_highelement = (uint16_t *)0xa002;

void save_game(uint16_t highscore, uint16_t highelement) {
    // Save the data to external RAM
    ENABLE_RAM_MBC5;
    f_highscore = highscore;
    f_highelement = highelement;
    DISABLE_RAM_MBC5;
}

void load_save(uint16_t * highscore, uint16_t* highelement) {
    // Load data from external RAM
    ENABLE_RAM_MBC5;

    if (saved[0] != 's') { // Check to see if the variable's ever been saved before
		*highscore=0;
		*highelement=0;
		saved[0] = 's'; // Assign saved an 's' value so the if statement isn't executed on next load
	}else{
        *highscore = f_highscore;
        *highelement = f_highelement;
    }
  
    DISABLE_RAM_MBC5;
    
  
}


