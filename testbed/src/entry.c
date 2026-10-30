#include "entry.h"
#include "game.h"

//TODO: remove this
#include "platform/platform.h"

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

    out_game->state = platform_allocate(sizeof(game_state), FALSE);

    return TRUE;
}