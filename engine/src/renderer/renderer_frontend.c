#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/kmemory.h"

struct platform_state;

//backend render context
static renderer_backend *backend = 0;

b8 renderer_init(const char *app_name, struct platform_state *plat_st) {
    backend = kallocate(sizeof(renderer_backend), MEM_TAG_RENDERER);

    //TODO: make this configurable
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_st, backend);
    backend->frame_num = 0;

    if (!backend->initialize(backend, app_name, plat_st)) {
        KFATAL("Renderer backend failed to initalize, shutting down!");
        return FALSE;
    }

    return TRUE;
}

void renderer_shutdown() {
    backend->shutdown(backend);
    kfree(backend, sizeof(renderer_backend), MEM_TAG_RENDERER);
}

void renderer_on_resized(u16 width, u16 height) {
    if (backend)
        backend->resized(backend, width, height);
    else
        KWARN("No renderer backend to resize.");
}

b8 renderer_begin_frame(f32 delta_time) {
    return backend->begin_frame(backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time) {
    b8 res = backend->end_frame(backend, delta_time);
    backend->frame_num++;
    return res;
}

b8 renderer_draw_frame(renderer_packet *pkt) {
    //if begin frame returned successfully, mid-frame operations may continue
    if (renderer_begin_frame(pkt->delta_time)) {
        //end frame; if this fails, it is likely unrecoverable
        b8 res = renderer_end_frame(pkt->delta_time);
        if (!res) {
            KERROR("renderer_end_frame() failed, application shutting down...");
            return FALSE;
        }
    }

    return TRUE;
}
