#pragma once

#include "renderer_types.inl"

//forward-declaring
struct platform_state;

b8 renderer_backend_create(
    renderer_backend_type type,
    struct platform_state *plat_st,
    renderer_backend *out_backend
);

void renderer_backend_destroy(renderer_backend *renderer_backend);
