#pragma once

#include "defines.h"

#include "core/asserts.h"

#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                  \
    {                                   \
        KASSERT(expr == VK_SUCCESS);    \
    }

typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR *formats;
    u32 present_mode_count;
    VkPresentModeKHR *present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_device {
    VkPhysicalDevice physical_dev;
    VkDevice logical_dev;
    vulkan_swapchain_support_info swapchain_support;
    
    i32 graphics_queue_index;
    i32 present_queue_index; 
    i32 transfer_queue_index;

    VkQueue graphics_queue;
    VkQueue present_queue; 
    VkQueue transfer_queue;

    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    VkFormat depth_format;
} vulkan_device;

typedef struct vulkan_image {
    VkImage handle;
    VkDeviceMemory mem;
    VkImageView view;
    u32 width;
    u32 height;
} vulkan_image;

typedef struct vulkan_swapchain {
    VkSurfaceFormatKHR img_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 img_count;
    VkImage *imgs;
    VkImageView *views;
    vulkan_image depth_attachment;
} vulkan_swapchain;

typedef struct vulkan_context {
    VkInstance inst;
    VkAllocationCallbacks *allocator;
    VkSurfaceKHR surface;

#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif

    vulkan_device dev;

    u32 framebuffer_width;
    u32 framebuffer_height;

    vulkan_swapchain swapchain;
    u32 img_ind;
    u32 curr_frame;
    b8 recreating_swch;

    i32 (*find_mem_ind)(u32 type_filter, u32 property_flags);
} vulkan_context;
