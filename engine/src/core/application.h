#pragma once

#include "defines.h"

//forward-declaring
struct game;

//app config
typedef struct app_config {
    //window starting position on x-axis, if applicable
    i16 start_pos_x;

    //window starting position on y-axis, if applicable
    i16 start_pos_y;

    //window starting width, if applicable
    i16 start_width;

    //window starting height, if applicable
    i16 start_height;
    
    //app name used in windowing, if applicable
    char *name;
} app_config;

KAPI b8 app_create(struct game *game_inst);

KAPI b8 app_run();