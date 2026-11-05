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

void vulkan_img_view_create(
    vulkan_context *context,
    VkFormat format,
    vulkan_image *img,
    VkImageAspectFlags aspect_flags
);

void vulkan_img_destroy(vulkan_context *context, vulkan_image *img);