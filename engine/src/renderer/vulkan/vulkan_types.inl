#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

typedef struct vulkan_context {
    VkInstance inst;
    VkAllocationCallbacks *allocator;
} vulkan_context;
