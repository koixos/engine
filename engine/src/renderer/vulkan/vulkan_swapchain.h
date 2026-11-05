#pragma once

#include "vulkan_types.inl"

void vulkan_swapchain_create(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *out_swch);

void vulkan_swapchain_recreate(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *swch);

void vulkan_swapchain_destroy(vulkan_context *context, vulkan_swapchain *swch);

b8 vulkan_swapchain_acquire_next_img_ind(
    vulkan_context *context,
    vulkan_swapchain *swch,
    u64 timeout_ns,
    VkSemaphore img_available_sem,
    VkFence fence,
    u32 *out_img_ind
);

void vulkan_swapchain_present(
    vulkan_context *context,
    vulkan_swapchain *swch,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_sem,
    u32 present_img_ind
);

