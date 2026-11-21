#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_command_buffer.h"
#include "vulkan_utils.h"

#include "core/logger.h"
#include "core/kmemory.h"

b8 vulkan_buffer_create(
    vulkan_context *context,
    u64 size,
    VkBufferUsageFlagBits usg,
    u32 mem_prop_flags,
    b8 bind_on_create,
    vulkan_buffer *out_buf
) {
    kzero_mem(out_buf, sizeof(vulkan_buffer));
    out_buf->total_size = size;
    out_buf->usg = usg;
    out_buf->mem_prop_flags = mem_prop_flags;

    VkBufferCreateInfo buf_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    buf_info.size = size;
    buf_info.usage = usg;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;   //bufs will be used only in one queue, not across multiple

    VK_CHECK(vkCreateBuffer(context->dev.logical_dev, &buf_info, context->allocator, &out_buf->handle));

    //gather mem reqs
    VkMemoryRequirements reqs;
    vkGetBufferMemoryRequirements(context->dev.logical_dev, out_buf->handle, &reqs);
    out_buf->mem_ind = context->find_mem_ind(reqs.memoryTypeBits, out_buf->mem_prop_flags);
    if (out_buf->mem_ind == -1) {
        KERROR("Unable to create Vulkan buffer because the required memory type index was not found.");
        return false;
    }

    //alloc mem info
    VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    alloc_info.allocationSize = reqs.size;
    alloc_info.memoryTypeIndex = (u32)out_buf->mem_ind;

    //alloc the mem
    VkResult res = vkAllocateMemory(context->dev.logical_dev, &alloc_info, context->allocator, &out_buf->mem);
    if (res != VK_SUCCESS) {
        KERROR("Unable to create Vulkan buffer because the required memory allocation failed: %i", res);
        return false;
    }

    if (bind_on_create)
        vulkan_buffer_bind(context, out_buf, 0);

    return true;
}

void vulkan_buffer_destroy(vulkan_context *context, vulkan_buffer *buf) {
    if (buf->mem) {
        vkFreeMemory(context->dev.logical_dev, buf->mem, context->allocator);
        buf->mem = 0;
    }

    if (buf->handle) {
        vkDestroyBuffer(context->dev.logical_dev, buf->handle, context->allocator);
        buf->handle = 0;
    }

    buf->total_size = 0;
    buf->usg = 0;
    buf->is_locked = false;
}

b8 vulkan_buffer_resize(
    vulkan_context *context, 
    u64 new_size, 
    vulkan_buffer *buf,
    VkQueue queue,
    VkCommandPool pool
) {
    //create new buf
    VkBufferCreateInfo buf_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    buf_info.size = new_size;
    buf_info.usage = buf->usg;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;   //bufs will be used only in one queue, not across multiple

    VkBuffer new_buf;
    VK_CHECK(vkCreateBuffer(context->dev.logical_dev, &buf_info, context->allocator, &new_buf));

    //gather mem reqs
    VkMemoryRequirements reqs;
    vkGetBufferMemoryRequirements(context->dev.logical_dev, new_buf, &reqs);

    //alloc mem info
    VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    alloc_info.allocationSize = reqs.size;
    alloc_info.memoryTypeIndex = (u32)buf->mem_ind;

    //alloc the mem
    VkDeviceMemory new_mem;
    VkResult res = vkAllocateMemory(context->dev.logical_dev, &alloc_info, context->allocator, &new_mem);
    if (res != VK_SUCCESS) {
        KERROR("Unable to resize Vulkan buffer because the required memory allocation failed: %i", res);
        return false;
    }

    //bind new buf's mem
    VK_CHECK(vkBindBufferMemory(context->dev.logical_dev, new_buf, new_mem, 0));

    //copy over the data
    vulkan_buffer_copy_to(context, pool, 0, queue, buf->handle, 0, new_buf, 0, buf->total_size);

    //make sure anything potentially using the old buf finished
    vkDeviceWaitIdle(context->dev.logical_dev);

    //destroy the old one
    if (buf->mem) {
        vkFreeMemory(context->dev.logical_dev, buf->mem, context->allocator);
        buf->mem = 0;
    }

    if (buf->handle) {
        vkDestroyBuffer(context->dev.logical_dev, buf->handle, context->allocator);
        buf->handle = 0;
    }

    buf->total_size = new_size;
    buf->mem = new_mem;
    buf->handle = new_buf;

    return true;
}

void vulkan_buffer_bind(vulkan_context *context, vulkan_buffer *buf, u64 offset) {
    VK_CHECK(vkBindBufferMemory(context->dev.logical_dev, buf->handle, buf->mem, offset));
}

void* vulkan_buffer_lock_mem(
    vulkan_context *context, 
    vulkan_buffer *buf,
    u64 offset,
    u64 size,
    u32 flags
) {
    void *data;
    VK_CHECK(vkMapMemory(context->dev.logical_dev, buf->mem, offset, size, flags, &data));
    return data;
}

void vulkan_buffer_unlock_mem(vulkan_context *context, vulkan_buffer *buf) {
    vkUnmapMemory(context->dev.logical_dev, buf->mem);
}

void vulkan_buffer_load_data(
    vulkan_context *context, 
    vulkan_buffer *buf,
    u64 offset,
    u64 size,
    u32 flags,
    const void *data
) {
    //locking
    void *data_ptr;
    VK_CHECK(vkMapMemory(context->dev.logical_dev, buf->mem, offset, size, flags, &data_ptr));
    //copying
    kcopy_mem(data_ptr, data, size);
    //unlocking
    vkUnmapMemory(context->dev.logical_dev, buf->mem);
}

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
) {
    vkQueueWaitIdle(queue);

    //create a one-time-use cmd buf
    vulkan_command_buffer temp_cmd_buf;
    vulkan_cmd_buf_allocate_begin_single_use(context, pool, &temp_cmd_buf);

    //prepare the copy cmd and add it to the cmd buf
    VkBufferCopy copy_region;
    copy_region.srcOffset = src_offset;
    copy_region.dstOffset = dest_offset;
    copy_region.size = size;

    vkCmdCopyBuffer(temp_cmd_buf.handle, src, dest, 1, &copy_region);

    //submit the buf for execution and wait for it to complete
    vulkan_cmd_buf_end_single_use(context, pool, &temp_cmd_buf, queue);
}