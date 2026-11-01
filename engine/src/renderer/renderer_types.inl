#pragma once

#include "defines.h"

typedef enum renderer_backend_type {
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX,
} renderer_backend_type;

typedef struct renderer_backend {
    //forward-declaring
    struct platform_state *plat_st;

    u64 frame_num;

    b8 (*initialize)(
        struct renderer_backend *backend,
        const char *app_name,
        struct platform_state *plat_st        
    );

    void (*shutdown)(struct renderer_backend *backend);

    void (*resized)(struct renderer_backend *backend, u16 width, u16 height);

    b8 (*begin_frame)(struct renderer_backend *backend, f32 delta_time);

    b8 (*end_frame)(struct renderer_backend *backend, f32 delta_time);

} renderer_backend;

typedef struct renderer_packet {
    f32 delta_time;
} renderer_packet;
