#pragma once

#include <defines.h>
#include <game_types.h>

#include <math/kmath.h>

typedef struct game_state {
    f32 delta_time;
    mat4 view;
    vec3 cam_pos;
    vec3 cam_euler;
    b8 cam_view_dirty;
} game_state;

b8 game_init(game *game_inst);
b8 game_update(game *game_inst, f32 delta_time);
b8 game_render(game *game_inst, f32 delta_time);

void game_on_resize(game *game_inst, u32 width, u32 height);