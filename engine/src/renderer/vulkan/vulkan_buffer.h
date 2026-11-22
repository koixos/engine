#pragma once

#include "vulkan_types.inl"

b8 vulkan_buffer_create(
    vulkan_context *context,
    u64 size,
    VkBufferUsageFlagBits usg,
    u32 mem_prop_flags,
    b8 bind_on_create,
    vulkan_buffer *out_buf
);

void vulkan_buffer_destroy(vulkan_context *context, vulkan_buffer *buf);

b8 vulkan_buffer_resize(vulkan_context *context, u64 new_size, vulkan_buffer *buf, VkQueue queue, VkCommandPool pool);

void vulkan_buffer_bind(vulkan_context *context, vulkan_buffer *buf, u64 offset);

void* vulkan_buffer_lock_mem(vulkan_context *context, vulkan_buffer *buf, u64 offset, u64 size, u32 flags);
void vulkan_buffer_unlock_mem(vulkan_context *context, vulkan_buffer *buf);

void vulkan_buffer_load_data(
    vulkan_context *context, 
    vulkan_buffer *buf,
    u64 offset,
    u64 size,
    u32 flags,
    const void *data
);

void vulkan_buffer_copy_to(
    vulkan_context *context, 
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer src,
    u64 src_offset,
    VkBuffer dest,
    u64 dest_offset,
    u64 size
);