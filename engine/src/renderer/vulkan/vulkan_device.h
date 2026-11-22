#pragma once

#include "vulkan_types.inl"

b8 vulkan_dev_create(vulkan_context *context);
void vulkan_dev_destroy(vulkan_context *context);

void vulkan_dev_query_swapchain_supp(
    VkPhysicalDevice physical_dev,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info *out_supp_info
);

b8 vulkan_dev_detect_depth_format(vulkan_device *dev);