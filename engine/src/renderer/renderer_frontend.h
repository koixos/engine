#pragma once

#include "renderer_types.inl"

//forward-declaring
struct static_mesh_data;
struct platform_state;

b8 renderer_init(const char *app_name, struct platform_state *plat_st);

void renderer_shutdown();

void renderer_on_resized(u16 width, u16 height);

b8 renderer_begin_frame(f32 delta_time);

b8 renderer_end_frame(f32 delta_time);

b8 renderer_draw_frame(renderer_packet *pkt);
