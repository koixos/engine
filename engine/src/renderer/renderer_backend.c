#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 renderer_backend_create(renderer_backend_type type, renderer_backend *out_backend) {
    if (type == RENDERER_BACKEND_TYPE_VULKAN) {
        out_backend->initialize = vulkan_renderer_backend_init;
        out_backend->shutdown = vulkan_renderer_backend_shutdown;
        out_backend->begin_frame = vulkan_renderer_backend_begin_frame;
        out_backend->end_frame = vulkan_renderer_backend_end_frame;
        out_backend->upd_global_st = vulkan_renderer_upd_global_st;
        out_backend->resized = vulkan_renderer_backend_on_resized;
        return true;
    }
    return false;
}

void renderer_backend_destroy(renderer_backend *renderer_backend) {
    renderer_backend->initialize = 0;
    renderer_backend->shutdown = 0;
    renderer_backend->begin_frame = 0;
    renderer_backend->end_frame = 0;
    renderer_backend->upd_global_st = 0;
    renderer_backend->resized = 0;
}