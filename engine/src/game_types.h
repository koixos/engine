#pragma once

#include "core/application.h"

/**
 * Represents the basic game state in a game.
 * Called for creation by the application.
 */
typedef struct game {
    //application configuration
    app_config app_config;

    //func ptr to game's init func
    b8 (*init)(struct game *game_inst);

    //func ptr to game's update func
    b8 (*update)(struct game *game_inst, f32 delta_time);
    
    //func ptr to game's render func
    b8 (*render)(struct game *game_inst, f32 delta_time);
    
    //func ptr to handle resizes, if applicable
    void (*on_resize)(struct game *game_inst, u32 width, u32 height);
    
    //game-specific game state - created and managed by the game
    void *state;
} game;
