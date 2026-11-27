#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/kmemory.h"

#include "math/kmath.h"

typedef struct renderer_sys_state {
    renderer_backend backend;  
    mat4 view;
    mat4 projection;
    f32 near_clip;
    f32 far_clip;
} renderer_sys_state;

static renderer_sys_state *state_ptr;

b8 renderer_sys_init(u64 *mem_requirement, void *st, const char *app_name) {
    *mem_requirement = sizeof(renderer_sys_state);
    if (!st)
        return true;
    state_ptr = st;
   
    //TODO: make this configurable
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, &state_ptr->backend);
    state_ptr->backend.frame_num = 0;

    if (!state_ptr->backend.initialize(&state_ptr->backend, app_name)) {
        KFATAL("Renderer backend failed to initalize, shutting down!");
        return false;
    }

    //def val.s for projection matrix
    state_ptr->near_clip = 0.1f;
    state_ptr->far_clip = 1000.0f;
    state_ptr->projection = mat4_perspective(deg2rad(45.0f), 1280/720.0f, 0.1f, 1000.0f);   

    //def val.s for view matrix
    state_ptr->view = mat4_translation((vec3){0, 0, 30.0f});
    state_ptr->view = mat4_inverse(state_ptr->view);
        

    KDEBUG("Renderer subsystem initialized.");
    return true;
}

void renderer_sys_shutdown(void *st) {
    if (state_ptr)
        state_ptr->backend.shutdown(&state_ptr->backend);
    state_ptr = 0;
}

void renderer_on_resized(u16 width, u16 height) {
    if (state_ptr) {
        state_ptr->projection = mat4_perspective(deg2rad(45.0f), width/(f32)height, state_ptr->near_clip, state_ptr->far_clip);
        state_ptr->backend.resized(&state_ptr->backend, width, height);
    } else {
        KWARN("No renderer backend to resize.");
    }
}

b8 renderer_begin_frame(f32 delta_time) {
    return state_ptr ? state_ptr->backend.begin_frame(&state_ptr->backend, delta_time) : false;
}

b8 renderer_end_frame(f32 delta_time) {
    if (!state_ptr)
        return false;
    b8 res = state_ptr->backend.end_frame(&state_ptr->backend, delta_time);
    state_ptr->backend.frame_num++;
    return res;
}

b8 renderer_draw_frame(renderer_packet *pkt) {
    //if begin frame returned successfully, mid-frame operations may continue
    if (renderer_begin_frame(pkt->delta_time)) {        
        state_ptr->backend.upd_global_st(state_ptr->projection, state_ptr->view, vec3_zero(), vec4_one(), 0);

        //mat4 model = mat4_translation((vec3){0, 0, 0});
        static f32 angle = 0.01f;
        angle += 0.01f;
        quat rot = quat_from_axis_angle(vec3_forward(), angle, false);
        mat4 model = quat_to_rotation_matrix(rot, vec3_zero());
        state_ptr->backend.upd_obj(model);

        //end frame; if this fails, it is likely unrecoverable
        b8 res = renderer_end_frame(pkt->delta_time);
        if (!res) {
            KERROR("renderer_end_frame() failed, application shutting down...");
            return false;
        }
    }
    return true;
}

void renderer_set_view(mat4 view) {
    state_ptr->view = view;
}
