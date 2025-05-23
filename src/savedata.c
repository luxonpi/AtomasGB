#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdint.h>
#include <string.h>

#include "savedata.h"

struct settings_rec {
    uint16_t saved;
    uint16_t highscore;
    uint16_t highelement;
};  

// Global settings structure
struct settings_rec game_settings;

// Map a secondary stats struct to the beginning of SRAM (0xA000) when using MBC5
struct settings_rec sram_stats;

uint16_t highscore = 0;
uint16_t highelement = 0;

void load_save(void) {
    ENABLE_RAM;
    SWITCH_RAM(0);  // Select RAM bank 0
    memcpy((void *)&game_settings, (void *)0xA000, sizeof(game_settings));
    DISABLE_RAM;

    if(game_settings.saved == 1953) {
        highscore = game_settings.highscore;
        highelement = game_settings.highelement;
    } else {
        highscore = 0;
        highelement = 0;
    }
}

// TODO: warning on failure to save?
void save_game(void) {
    game_settings.highscore = highscore;
    game_settings.highelement = highelement;
    game_settings.saved = 1953;

    ENABLE_RAM;
    SWITCH_RAM(0);  // Select RAM bank 0
    memcpy((void *)0xA000, (void *)&game_settings, sizeof(game_settings));
    DISABLE_RAM;
}