#include "vulkan_command_buffer.h"

#include "core/kmemory.h"

void vulkan_cmd_buf_allocate(
    vulkan_context *context,
    VkCommandPool pool,
    b8 is_primary,
    vulkan_command_buffer *out_cmd_buf
) {
    kzero_mem(out_cmd_buf, sizeof(out_cmd_buf));
    out_cmd_buf->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    
    VkCommandBufferAllocateInfo allocate_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocate_info.commandPool = pool;
    allocate_info.level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = 0;

    out_cmd_buf->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_cmd_buf_free(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *cmd_buf
) {
    vkFreeCommandBuffers(
        context->dev.logical_dev,
        pool,
        1,
        &cmd_buf->handle
    );

    cmd_buf->handle = 0;
    cmd_buf->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void vulkan_cmd_buf_begin(
    vulkan_command_buffer *cmd_buf,
    b8 is_single_use,
    b8 is_renderpass_continue,
    b8 is_simultaneous_use
) {
    VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = 0;
    
    if (is_single_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (is_renderpass_continue)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if (is_simultaneous_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CHECK(vkBeginCommandBuffer(cmd_buf->handle, &begin_info));
    cmd_buf->state = COMMAND_BUFFER_STATE_RECORDING;
}

void vulkan_cmd_buf_end(vulkan_command_buffer *cmd_buf) {
    VK_CHECK(vkEndCommandBuffer(cmd_buf->handle));
    cmd_buf->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void vulkan_cmd_buf_upd_submitted(vulkan_command_buffer *cmd_buf) {
    cmd_buf->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void vulkan_cmd_buf_reset(vulkan_command_buffer *cmd_buf) {
    cmd_buf->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_cmd_buf_allocate_begin_single_use(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *out_cmd_buf
) {
    vulkan_cmd_buf_allocate(context, pool, TRUE, out_cmd_buf);
    vulkan_cmd_buf_begin(out_cmd_buf, TRUE, FALSE, FALSE);
}

void vulkan_cmd_buf_end_single_use(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *cmd_buf,
    VkQueue queue
) {
    vulkan_cmd_buf_end(cmd_buf);

    //submit the queue
    VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf->handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));

    //wait for it to finish
    VK_CHECK(vkQueueWaitIdle(queue));

    //free the cmd buf
    vulkan_cmd_buf_free(context, pool, cmd_buf);
}