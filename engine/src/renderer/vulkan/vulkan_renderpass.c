#include "vulkan_renderpass.h"

#include "core/kmemory.h"

void vulkan_renderpass_create(
    vulkan_context *context,
    vulkan_renderpass *out_renderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil
) {
    //main subpass
    VkSubpassDescription subpass = {}; 
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    //attachments
    //TODO: make this configurable
    u32 attachment_description_count = 2;
    VkAttachmentDescription attachment_descriptions[attachment_description_count];

    //color attachment
    VkAttachmentDescription color_attachment;
    color_attachment.format = context->swapchain.img_format.format; //TODO: make this configurable
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //dont expect any particular layout before render pass starts
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //transitioned to after the render pass
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.flags = 0;

    attachment_descriptions[0] = color_attachment;

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.attachment = 0;    //attachment description arr ind
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    //depth attachment, if there is one
    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = context->dev.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachment_descriptions[1] = depth_attachment;

    VkAttachmentReference depth_attachment_ref;
    depth_attachment_ref.attachment = 1; 
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    subpass.pDepthStencilAttachment = &depth_attachment_ref;
 
    //TODO: other attachment types such as inp, resolve, preserve etc

    //input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    //attachments used for multisampling clr attachments
    subpass.pResolveAttachments = 0;

    //attachments not used in this subpass, but must be preserved for the next one
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    //render pass dependencies
    //TODO: make this configurable
    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    //render pass create
    VkRenderPassCreateInfo renderpass_create_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO }; 
    renderpass_create_info.attachmentCount = attachment_description_count;
    renderpass_create_info.pAttachments = attachment_descriptions;
    renderpass_create_info.subpassCount = 1;
    renderpass_create_info.pSubpasses = &subpass;
    renderpass_create_info.dependencyCount = 1;
    renderpass_create_info.pDependencies = &dependency;
    renderpass_create_info.pNext = 0;
    renderpass_create_info.flags = 0;

    VK_CHECK(vkCreateRenderPass(
        context->dev.logical_dev,
        &renderpass_create_info,
        context->allocator,
        &out_renderpass->handle
    ));
}

void vulkan_renderpass_destroy(vulkan_context *context, vulkan_renderpass *renderpass) {
    if (renderpass && renderpass->handle) {
        vkDestroyRenderPass(context->dev.logical_dev, renderpass->handle, context->allocator);
        renderpass->handle = 0;
    }
}

void vulkan_renderpass_begin(
    vulkan_command_buffer *cmd_buf,
    vulkan_renderpass *renderpass,
    VkFramebuffer framebuf
) {
    VkRenderPassBeginInfo begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    begin_info.renderPass = renderpass->handle;
    begin_info.framebuffer = framebuf;
    begin_info.renderArea.offset.x = renderpass->x;
    begin_info.renderArea.offset.y = renderpass->y;
    begin_info.renderArea.extent.width = renderpass->w;
    begin_info.renderArea.extent.height = renderpass->h;

    VkClearValue clear_vals[2];
    kzero_mem(clear_vals, sizeof(VkClearValue) * 2);
    clear_vals[0].color.float32[0] = renderpass->r;
    clear_vals[0].color.float32[1] = renderpass->g;
    clear_vals[0].color.float32[2] = renderpass->b;
    clear_vals[0].color.float32[3] = renderpass->a;
    clear_vals[0].depthStencil.depth = renderpass->depth;
    clear_vals[0].depthStencil.stencil = renderpass->stencil;

    begin_info.clearValueCount = 2;
    begin_info.pClearValues = clear_vals;

    vkCmdBeginRenderPass(cmd_buf->handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    cmd_buf->state = COMMAND_BUFFER_STATE_IN_RENDERPASS;
}

void vulkan_renderpass_end(vulkan_command_buffer *cmd_buf, vulkan_renderpass *renderpass) {
    vkCmdEndRenderPass(cmd_buf->handle);
    cmd_buf->state = COMMAND_BUFFER_STATE_RECORDING;
}