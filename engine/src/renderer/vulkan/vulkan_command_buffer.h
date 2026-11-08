#pragma once

#include "vulkan_types.inl"

void vulkan_cmd_buf_allocate(
    vulkan_context *context,
    VkCommandPool pool,
    b8 is_primary,
    vulkan_command_buffer *out_cmd_buf
);

void vulkan_cmd_buf_free(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *cmd_buf
);

void vulkan_cmd_buf_begin(
    vulkan_command_buffer *cmd_buf,
    b8 is_single_use,
    b8 is_renderpass_continue,
    b8 is_simultaneous_use
);

void vulkan_cmd_buf_end(vulkan_command_buffer *cmd_buf);

void vulkan_cmd_buf_update_submitted(vulkan_command_buffer *cmd_buf);

void vulkan_cmd_buf_reset(vulkan_command_buffer *cmd_buf);

/**
 * Allocates and begins recording to out_cmd_buf
 */
void vulkan_cmd_buf_allocate_begin_single_use(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *out_cmd_buf
);

/**
 * Ends recording, submits to and waits for queue op
 *  and frees the provided cmd buf
 */
void vulkan_cmd_buf_end_single_use(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *cmd_buf,
    VkQueue queue
);