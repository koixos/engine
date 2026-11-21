#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/kmemory.h"

typedef struct renderer_sys_state {
    renderer_backend backend;
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

    KDEBUG("Renderer subsystem initialized.");
    return true;
}

void renderer_sys_shutdown(void *st) {
    if (state_ptr)
        state_ptr->backend.shutdown(&state_ptr->backend);
    state_ptr = 0;
}

void renderer_on_resized(u16 width, u16 height) {
    if (state_ptr)
        state_ptr->backend.resized(&state_ptr->backend, width, height);
    else
        KWARN("No renderer backend to resize.");
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
        //end frame; if this fails, it is likely unrecoverable
        b8 res = renderer_end_frame(pkt->delta_time);
        if (!res) {
            KERROR("renderer_end_frame() failed, application shutting down...");
            return false;
        }
    }
    return true;
}
