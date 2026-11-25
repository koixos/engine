#pragma once

#include "vulkan_types.inl"

#include "renderer/renderer_backend.h"

b8 vulkan_backend_init(renderer_backend *backend, const char *app_name);
void vulkan_backend_shutdown(renderer_backend *backend);

void vulkan_backend_on_resized(renderer_backend *backend, u16 width, u16 height);

b8 vulkan_backend_begin_frame(renderer_backend *backend, f32 delta_time);
b8 vulkan_backend_end_frame(renderer_backend *backend, f32 delta_time);
void vulkan_backend_upd_global_st(mat4 projection, mat4 view, vec3 view_pos, vec4 ambient_clr, i32 mode);

void vulkan_backend_upd_obj(mat4 model);
