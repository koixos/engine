#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 renderer_backend_create(renderer_backend_type type, renderer_backend *out_backend) {
    if (type == RENDERER_BACKEND_TYPE_VULKAN) {
        out_backend->initialize = vulkan_backend_init;
        out_backend->shutdown = vulkan_backend_shutdown;
        out_backend->begin_frame = vulkan_backend_begin_frame;
        out_backend->end_frame = vulkan_backend_end_frame;
        out_backend->upd_global_st = vulkan_backend_upd_global_st;
        out_backend->resized = vulkan_backend_on_resized;
        out_backend->upd_obj = vulkan_backend_upd_obj;
        out_backend->create_texture = vulkan_backend_create_texture;
        out_backend->destroy_texture = vulkan_backend_destroy_texture;
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
    renderer_backend->upd_obj = 0;
    renderer_backend->create_texture = 0;
    renderer_backend->destroy_texture = 0;
}