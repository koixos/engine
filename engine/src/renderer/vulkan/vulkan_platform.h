#pragma once

#include "defines.h"

struct platform_state;
struct vulkan_context;

/**
 * Appends the names of required extensions for this platform
 * to the names_darr, which should be created and passed in.
 */
void platform_get_required_ext_names(const char ***names_darr);

b8 platform_create_vulkan_surface(
    struct platform_state *plat_st,
    struct vulkan_context *context
);