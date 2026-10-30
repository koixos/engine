#include "entry.h"
#include "game.h"

b8 create_game(game *out_game) {
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "ISIM KOYMAM LAZIM Engine Testbed";

    out_game->init = game_init;
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->on_resize = game_on_resize;

    //create game state
    out_game->state = kallocate(sizeof(game_state), MEM_TAG_GAME);

    return TRUE;
}