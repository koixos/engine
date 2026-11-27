#include "game.h"

#include <core/logger.h>
#include <core/input.h>
#include <core/kmemory.h>

//TODO: remove this
#include <renderer/renderer_frontend.h>

static void recalculate_view_matrix(game_state *st);
static void cam_pitch(game_state *st, f32 amount);
static void cam_yaw(game_state *st, f32 amount);

b8 game_init(game *game_inst) {
    KDEBUG("game init() called!");

    game_state *st = (game_state *)game_inst->state;

    st->cam_pos = (vec3){0, 0, 30.0f};
    st->cam_euler = vec3_zero();
    st->cam_view_dirty = true;

    st->view = mat4_translation(st->cam_pos);
    st->view = mat4_inverse(st->view);

    return true;
}

b8 game_update(game *game_inst, f32 delta_time) {
    //TODO: if more than one game inst, below static will be a problem
    static u64 alloc_count = 0;
    u64 prev_alloc_count = alloc_count;
    alloc_count = get_mem_alloc_count();
    if (input_is_key_up('M') && input_was_key_down('M')) 
        KDEBUG("Allocations: %llu (%llu this frame)", alloc_count, alloc_count - prev_alloc_count);

    game_state *st = (game_state *)game_inst->state;
    
    //TODO: temp hack to move camaround
    if (input_is_key_down('A') || input_is_key_down(KEY_LEFT)) 
        cam_yaw(st, -1.0f * delta_time);
    
    if (input_is_key_down('D') || input_is_key_down(KEY_RIGHT)) 
        cam_yaw(st, 1.0f * delta_time);

    if (input_is_key_down(KEY_UP)) 
        cam_pitch(st, -1.0f * delta_time);
    
    if (input_is_key_down(KEY_DOWN)) 
        cam_pitch(st, 1.0f * delta_time);
    
    f32 temp_move_speed = 30.0f;
    vec3 vel = vec3_zero();

    if (input_is_key_down('W')) {
        vec3 forward = mat4_forward(st->view);
        vel = vec3_add(vel, forward);
    }
    
    if (input_is_key_down('S')) {
        vec3 backward = mat4_backward(st->view);
        vel = vec3_add(vel, backward);
    }

    if (input_is_key_down('Q')) {
        vec3 left = mat4_left(st->view);
        vel = vec3_add(vel, left);
    }
    
    if (input_is_key_down('E')) {
        vec3 right = mat4_right(st->view);
        vel = vec3_add(vel, right);
    }

    if (input_is_key_down(KEY_SPACE))
        vel.y += 1.0f;

    if (input_is_key_down('X'))
        vel.y -= 1.0f;

    vec3 z = vec3_zero();
    if (!vec3_compare(z, vel, 0.0002f)) {
        //be sure to normalize the vel vec before applying speed
        vec3_normalize(&vel);
        st->cam_pos.x += vel.x * temp_move_speed * delta_time;
        st->cam_pos.y += vel.y * temp_move_speed * delta_time;
        st->cam_pos.z += vel.z * temp_move_speed * delta_time;
        st->cam_view_dirty = true;
    }

    recalculate_view_matrix(st);
    //TODO: dont do like this
    renderer_set_view(st->view);

    return true;
}

b8 game_render(game *game_inst, f32 delta_time) {
    return true;
}

void game_on_resize(game *game_inst, u32 width, u32 height) { }

void recalculate_view_matrix(game_state *st) {
    if (st->cam_view_dirty) {
        mat4 rot = mat4_euler_xyz(st->cam_euler.x, st->cam_euler.y, st->cam_euler.z);   //pitch, yaw, roll
        mat4 translation = mat4_translation(st->cam_pos);
        
        st->view = mat4_mul(rot, translation);
        st->view = mat4_inverse(st->view);

        st->cam_view_dirty = false;
    }
}

void cam_pitch(game_state *st, f32 amount) {
    st->cam_euler.x += amount;

    //clamp to avoid gimball lock
    f32 limit = deg2rad(89.0f);
    st->cam_euler.x = KCLAMP(st->cam_euler.x, -limit, limit);

    st->cam_view_dirty = true;
}

void cam_yaw(game_state *st, f32 amount) {
    st->cam_euler.y += amount;
    st->cam_view_dirty = true;
}