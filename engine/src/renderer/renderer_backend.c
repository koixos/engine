#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 renderer_backend_create(
    renderer_backend_type type,
    struct platform_state *plat_st,
    renderer_backend *out_backend
) {
    out_backend->plat_st = plat_st;

    if (type == RENDERER_BACKEND_TYPE_VULKAN) {
        out_backend->initialize = vulkan_renderer_backend_init;
        out_backend->shutdown = vulkan_renderer_backend_shutdown;
        out_backend->begin_frame = vulkan_renderer_backend_begin_frame;
        out_backend->end_frame = vulkan_renderer_backend_end_frame;
        out_backend->resized = vulkan_renderer_backend_on_resized;
        
        return TRUE;
    }

    return FALSE;
}

void renderer_backend_destroy(renderer_backend *renderer_backend) {
    renderer_backend->initialize = 0;
    renderer_backend->shutdown = 0;
    renderer_backend->begin_frame = 0;
    renderer_backend->end_frame = 0;
    renderer_backend->resized = 0;
}