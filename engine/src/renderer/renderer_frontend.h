#pragma once

#include "renderer_types.inl"

b8 renderer_sys_init(u64 *mem_requirement, void *st, const char *app_name);
void renderer_sys_shutdown(void *st);

void renderer_on_resized(u16 width, u16 height);

b8 renderer_begin_frame(f32 delta_time);
b8 renderer_end_frame(f32 delta_time);
b8 renderer_draw_frame(renderer_packet *pkt);

//TODO: should remove the KAPI
KAPI void renderer_set_view(mat4 view);