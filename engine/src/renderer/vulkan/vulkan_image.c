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

    //bind the mem
    //TODO: configurable mem offset
    VK_CHECK(vkBindImageMemory(context->dev.logical_dev, out_img->handle, out_img->mem, 0));

    //create view
    if (create_view) {
        out_img->view = 0;
        vulkan_img_view_create(context, format, out_img, view_aspect_flags);
    }
}

void vulkan_img_view_create(
    vulkan_context *context,
    VkFormat format,
    vulkan_image *img,
    VkImageAspectFlags aspect_flags
) {
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