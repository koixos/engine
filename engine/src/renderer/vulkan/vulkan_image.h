#pragma once

#include "vulkan_types.inl"

void vulkan_img_create(
    vulkan_context *context,
    VkImageType img_type,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags mem_flags,
    b32 create_view,
    VkImageAspectFlags view_aspect_flags,
    vulkan_image *out_img
);

void vulkan_img_destroy(vulkan_context *context, vulkan_image *img);

void vulkan_img_view_create(vulkan_context *context, VkFormat format, vulkan_image *img, VkImageAspectFlags aspect_flags);

/**
 * Transitions the given img from old_layout to new_layout.
 */
void vulkan_img_transition_layout(
    vulkan_context *context,
    vulkan_command_buffer *cmd_buf,
    vulkan_image *img,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout    
);

/**
 * Copies data in buf to the given img. 
 */
void vulkan_img_copy_from_buf(vulkan_context *context, vulkan_image *img, VkBuffer buf, vulkan_command_buffer *cmd_buf);