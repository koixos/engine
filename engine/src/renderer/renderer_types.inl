#pragma once

#include "defines.h"

#include "math/math_types.h"

typedef enum renderer_backend_type {
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX,
} renderer_backend_type;

//uniform objs need to be 256 bytes in size, thus we need to pad it by adding some reserved space
typedef struct global_uniform_obj {
    mat4 projection;    //64 bytes
    mat4 view;          //64 bytes
    mat4 m_reserved0;   //64 bytes, reserved for future use
    mat4 m_reserved1;   //64 bytes, reserved for future use
} global_uniform_obj;

typedef struct renderer_backend {
    u64 frame_num;

    b8 (*initialize)(struct renderer_backend *backend, const char *app_name);
    void (*shutdown)(struct renderer_backend *backend);
    void (*resized)(struct renderer_backend *backend, u16 width, u16 height);

    b8 (*begin_frame)(struct renderer_backend *backend, f32 delta_time);
    b8 (*end_frame)(struct renderer_backend *backend, f32 delta_time);
    void (*upd_global_st)(mat4 projection, mat4 view, vec3 view_pos, vec4 ambient_clr, i32 mode);


} renderer_backend;

typedef struct renderer_packet {
    f32 delta_time;
} renderer_packet;
