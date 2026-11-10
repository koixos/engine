#include "vulkan_framebuffer.h"

#include "core/kmemory.h"

void vulkan_framebuf_create(
    vulkan_context *context,
    vulkan_renderpass *rp,
    u32 width,
    u32 height,
    u32 attachment_count,
    VkImageView *attachments,
    vulkan_framebuffer *out_framebuf
) {
    //take a copy of the attachments, renderpass, and attachment count
    out_framebuf->attachments = kallocate(sizeof(VkImageView) * attachment_count, MEM_TAG_RENDERER);
    for (u32 i = 0; i < attachment_count; i++)
        out_framebuf->attachments[i] = attachments[i];
    out_framebuf->renderpass = rp;
    out_framebuf->attachment_count = attachment_count;

    VkFramebufferCreateInfo framebuf_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    framebuf_create_info.renderPass = rp->handle;
    framebuf_create_info.attachmentCount = attachment_count;
    framebuf_create_info.pAttachments = out_framebuf->attachments;
    framebuf_create_info.width = context->swapchain.extent.width;
    framebuf_create_info.height = context->swapchain.extent.height;
    framebuf_create_info.layers = 1;

    VK_CHECK(vkCreateFramebuffer(
        context->dev.logical_dev,
        &framebuf_create_info,
        context->allocator,
        &out_framebuf->handle
    ));
}

void vulkan_framebuf_destroy(vulkan_context *context, vulkan_framebuffer *framebuf) {
    vkDestroyFramebuffer(context->dev.logical_dev, framebuf->handle, context->allocator);
    if (framebuf->attachments) {
        kfree(framebuf->attachments, sizeof(VkImageView) * framebuf->attachment_count, MEM_TAG_RENDERER);
        framebuf->attachments = 0;
    }
    framebuf->handle = 0;
    framebuf->attachment_count = 0;
    framebuf->renderpass = 0;
}