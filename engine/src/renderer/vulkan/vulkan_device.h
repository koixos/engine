#pragma once

#include "vulkan_types.inl"

typedef struct vulkan_physical_device_requirements {
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    const char **dev_ext_names; //darray
    b8 sampler_anisotropy;
    b8 discrete_gpu;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info {
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

b8 vulkan_dev_create(vulkan_context *context);

void vulkan_dev_destroy(vulkan_context *context);

b8 select_physical_dev(vulkan_context *context);

b8 physical_dev_meets_reqs(
    VkPhysicalDevice dev,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties *props,
    const VkPhysicalDeviceFeatures *features,
    const vulkan_physical_device_requirements *reqs,
    vulkan_physical_device_queue_family_info *out_queue_fam_info,
    vulkan_swapchain_support_info *out_swapchain_supp
);

void vulkan_dev_query_swapchain_supp(
    VkPhysicalDevice physical_dev,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info *out_supp_info
);

b8 vulkan_dev_detect_depth_format(vulkan_device *dev);