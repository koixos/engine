#pragma once

#include "defines.h"

/**
 * Appends the names of required extensions for the current platform
 *  to the names_darr, which should be created and passed in.
 */
void platform_get_required_ext_names(const char ***names_darr);

b8 platform_create_vulkan_surface(struct vulkan_context *context);