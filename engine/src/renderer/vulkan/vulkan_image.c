#include "vulkan_image.h"
#include "vulkan_device.h"

#include "core/logger.h"
#include "core/kmemory.h"

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
) {
    //copy params
    out_img->width = width;
    out_img->height = height;

    //creation info
    VkImageCreateInfo img_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    img_create_info.imageType = VK_IMAGE_TYPE_2D;
    img_create_info.extent.width = width;
    img_create_info.extent.height = height;
    img_create_info.extent.depth = 1;   //TODO: supp configurable depth
    img_create_info.mipLevels = 4;      //TODO: supp mip mapping
    img_create_info.arrayLayers = 1;    //TODO: # of layers in the img
    img_create_info.format = format;
    img_create_info.tiling = tiling;
    img_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    img_create_info.usage = usage;
    img_create_info.samples = VK_SAMPLE_COUNT_1_BIT;            //TODO: configurable smaple count
    img_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;    //TODO: configurable sharing mode

    VK_CHECK(vkCreateImage(context->dev.logical_dev, &img_create_info, context->allocator, &out_img->handle));

    //query mem reqs
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(context->dev.logical_dev, out_img->handle, &mem_reqs);

    i32 mem_type = context->find_mem_ind(mem_reqs.memoryTypeBits, mem_flags);
    if (mem_type == -1)
        KERROR("Required memory type not found - image not valid.");

    //alloc mem
    VkMemoryAllocateInfo mem_alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    mem_alloc_info.allocationSize = mem_reqs.size;
    mem_alloc_info.memoryTypeIndex = mem_type;
    VK_CHECK(vkAllocateMemory(context->dev.logical_dev, &mem_alloc_info, context->allocator, &out_img->mem));

    //bind the mem - TODO: configurable mem offset
    VK_CHECK(vkBindImageMemory(context->dev.logical_dev, out_img->handle, out_img->mem, 0));

    //create view
    if (create_view) {
        out_img->view = 0;
        vulkan_img_view_create(context, format, out_img, view_aspect_flags);
    }
}

void vulkan_img_destroy(vulkan_context *context, vulkan_image *img) {
    if (img->view) {
        vkDestroyImageView(context->dev.logical_dev, img->view, context->allocator);
        img->view = 0;
    }

    if (img->mem) {
        vkFreeMemory(context->dev.logical_dev, img->mem, context->allocator);
        img->mem = 0;
    }

    if (img->handle) {
        vkDestroyImage(context->dev.logical_dev, img->handle, context->allocator);
        img->handle = 0;
    }
}

void vulkan_img_view_create(vulkan_context *context, VkFormat format, vulkan_image *img, VkImageAspectFlags aspect_flags) {
    VkImageViewCreateInfo view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    view_create_info.image = img->handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;  //TODO: make configurable
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = aspect_flags;

    //TODO: make configurable
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->dev.logical_dev, &view_create_info, context->allocator, &img->view));
}

void vulkan_img_transition_layout(
    vulkan_context *context,
    vulkan_command_buffer *cmd_buf,
    vulkan_image *img,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout    
) {
    VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = context->dev.graphics_queue_fam_ind;
    barrier.dstQueueFamilyIndex = context->dev.graphics_queue_fam_ind;
    barrier.image = img->handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags src_stage, dst_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        //dont care about the old layput - transition to optimal layout (for underlying implementation)
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  //dont care what stage the pipeline is in at the start
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;     //used for copying
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        //transitioning from a transfer dest layout to a shader-readonly layout
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;         //from a copying stage to...
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // the fragment stage
    } else {
        KFATAL("Unsupported layout transition!");
        return;
    }

    vkCmdPipelineBarrier(cmd_buf->handle, src_stage, dst_stage, 0, 0, 0, 0, 0, 1, &barrier);
}

void vulkan_img_copy_from_buf(vulkan_context *context, vulkan_image *img, VkBuffer buf, vulkan_command_buffer *cmd_buf) {
    //region to copy
    VkBufferImageCopy region;
    kzero_mem(&region, sizeof(VkBufferImageCopy));
    
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageExtent.width = img->width;
    region.imageExtent.height = img->height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(cmd_buf->handle, buf, img->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}