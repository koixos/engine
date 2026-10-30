#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

//externally-defined func to create a game
extern b8 create_game(game *out_game);

/**
 * The main entry point of the application.
 */
int main(void) {
    //request game instance from app
    game game_inst;
    if (!create_game(&game_inst)) {
        KFATAL("Could not create game!");
        return -1;
    }

    //ensure func ptrs exist
    if (!game_inst.init || !game_inst.update || !game_inst.render || !game_inst.on_resize) {
        KFATAL("The game's function pointers must be assigned!");
        return -2;
    }

    //initialization
    if (!app_create(&game_inst)) {
        KINFO("Application failed to create");
        return 1;
    }

    //begin game loop
    if (!app_run()) {
        KINFO("Application did not shutdown gracefully");
        return 2;
    }

    return 0;
}