#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_image.h"

#include "core/logger.h"
#include "core/kmemory.h"

void create(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *swch);

void destroy(vulkan_context *context, vulkan_swapchain *swch);

void vulkan_swapchain_create(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *out_swch) {
    create(context, width, height, out_swch);
}

void vulkan_swapchain_recreate(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *swch) {
    destroy(context, swch);
    create(context, width, height, swch);
}

void vulkan_swapchain_destroy(vulkan_context *context, vulkan_swapchain *swch) {
    destroy(context, swch);
}

b8 vulkan_swapchain_acquire_next_img_ind(
    vulkan_context *context,
    vulkan_swapchain *swch,
    u64 timeout_ns,
    VkSemaphore img_available_sem,
    VkFence fence,
    u32 *out_img_ind
) {
    VkResult res = vkAcquireNextImageKHR(
        context->dev.logical_dev,
        swch->handle,
        timeout_ns,
        img_available_sem,
        fence,
        out_img_ind
    );

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        //trigger swapchain recreation, then boot out of render loop
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swch);
        return FALSE;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        KFATAL("Failed to acquire swapchain image!");
        return FALSE;
    }

    return TRUE;
}

void vulkan_swapchain_present(
    vulkan_context *context,
    vulkan_swapchain *swch,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_sem,
    u32 present_img_ind
) {
    //return the img to the swapch for presentation
    VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_sem;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swch->handle;
    present_info.pImageIndices = &present_img_ind;
    present_info.pResults = 0;

    VkResult res = vkQueuePresentKHR(present_queue, &present_info);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) 
        //trigger swapchain recreation, if swch is OOD or a framebuf resize has occurred
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swch);
    else if (res != VK_SUCCESS) 
        KFATAL("Failed to present swapchain image!");

    //increment and loop the ind
    context->curr_frame = (context->curr_frame + 1) % swch->max_frames_in_flight;
}

void create(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *swch) {
    VkExtent2D swch_extent = { width, height };
    swch->max_frames_in_flight = 2; //triple-buffering mode

    //choose a swap surface format
    b8 found = FALSE;
    for (u32 i = 0; i < context->dev.swapchain_support.format_count; i++) {
        VkSurfaceFormatKHR format = context->dev.swapchain_support.formats[i];
        //preferred formats
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                swch->img_format = format;
                found = TRUE;
                break;
        }
    }

    //if none of proper format is found on dev, select the first format available 
    if (!found)
        swch->img_format = context->dev.swapchain_support.formats[0];
    
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < context->dev.swapchain_support.present_mode_count; i++) {
        VkPresentModeKHR mode = context->dev.swapchain_support.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = mode;
            break;
        }
    }

    //re-query swch supp
    vulkan_dev_query_swapchain_supp(
        context->dev.physical_dev,
        context->surface,
        &context->dev.swapchain_support
    );

    //check swch extent
    if (context->dev.swapchain_support.capabilities.currentExtent.width != UINT32_MAX)
        swch_extent = context->dev.swapchain_support.capabilities.currentExtent;

    //clamp to the value allowed by GPU
    VkExtent2D min = context->dev.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context->dev.swapchain_support.capabilities.maxImageExtent;
    swch_extent.width = KCLAMP(swch_extent.width, min.width, max.width);
    swch_extent.height = KCLAMP(swch_extent.height, min.height, max.height);

    u32 img_count = context->dev.swapchain_support.capabilities.minImageCount + 1;
    if (context->dev.swapchain_support.capabilities.maxImageCount > 0 &&
        img_count > context->dev.swapchain_support.capabilities.maxImageCount)
            img_count = context->dev.swapchain_support.capabilities.maxImageCount;
    
    swch->extent = swch_extent;

    //swch create info
    VkSwapchainCreateInfoKHR swch_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swch_create_info.surface = context->surface;
    swch_create_info.minImageCount = img_count;
    swch_create_info.imageFormat = swch->img_format.format;
    swch_create_info.imageColorSpace = swch->img_format.colorSpace;
    swch_create_info.imageExtent = swch_extent;
    swch_create_info.imageArrayLayers = 1;
    swch_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //setup the queue fam indices
    if (context->dev.graphics_queue_index != context->dev.present_queue_index) {
        u32 queue_fam_indices[] = {
            (u32)context->dev.graphics_queue_index,
            (u32)context->dev.present_queue_index
        };
        swch_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swch_create_info.queueFamilyIndexCount = 2;
        swch_create_info.pQueueFamilyIndices = queue_fam_indices;
    } else {
        swch_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swch_create_info.queueFamilyIndexCount = 0;
        swch_create_info.pQueueFamilyIndices = 0;
    }

    swch_create_info.preTransform = context->dev.swapchain_support.capabilities.currentTransform;
    swch_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swch_create_info.presentMode = present_mode;
    swch_create_info.clipped = VK_TRUE;
    swch_create_info.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(
        context->dev.logical_dev,
        &swch_create_info,
        context->allocator,
        &swch->handle
    ));

    //start with a zero frame ind
    context->curr_frame = 0;

    //imgs
    swch->img_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->dev.logical_dev, swch->handle, &swch->img_count, 0));
    if (!swch->imgs)
        swch->imgs = (VkImage *)kallocate(sizeof(VkImage) * swch->img_count, MEM_TAG_RENDERER);
    if (!swch->views)
        swch->views = (VkImageView *)kallocate(sizeof(VkImageView) * swch->img_count, MEM_TAG_RENDERER);
    VK_CHECK(vkGetSwapchainImagesKHR(context->dev.logical_dev, swch->handle, &swch->img_count, swch->imgs));

    //views
    for (u32 i = 0; i < swch->img_count; i++) {
        VkImageViewCreateInfo view_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        view_info.image = swch->imgs[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swch->img_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(context->dev.logical_dev, &view_info, context->allocator, &swch->views[i]));
    }

    //depth resources
    if (!vulkan_dev_detect_depth_format(&context->dev)) {
        context->dev.depth_format = VK_FORMAT_UNDEFINED;
        KFATAL("Failed to find a supported format!");
    }

    //create depth img and its view
    vulkan_img_create(
        context,
        VK_IMAGE_TYPE_2D,
        swch_extent.width,
        swch_extent.height,
        context->dev.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        TRUE,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swch->depth_attachment
    );

    KINFO("Swapchain created successfully.");
}

void destroy(vulkan_context *context, vulkan_swapchain *swch) {
    vkDeviceWaitIdle(context->dev.logical_dev);
    
    vulkan_img_destroy(context, &swch->depth_attachment);

    //only destroy the views, not the imgs since those are owned by
    // swch and are thus destroyed when it is
    for (u32 i = 0; i < swch->img_count; i++)
        vkDestroyImageView(context->dev.logical_dev, swch->views[i], context->allocator);

    vkDestroySwapchainKHR(context->dev.logical_dev, swch->handle, context->allocator);
}




