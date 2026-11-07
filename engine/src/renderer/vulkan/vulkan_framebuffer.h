#pragma once

#include "vulkan_types.inl"

void vulkan_framebuf_create(
    vulkan_context *context,
    vulkan_renderpass *rp,
    u32 width,
    u32 height,
    u32 attachment_count,
    VkImageView *attachments,
    vulkan_framebuffer *out_framebuf
);

void vulkan_framebuf_destroy(vulkan_context *context, vulkan_framebuffer *framebuf);