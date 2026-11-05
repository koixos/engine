#include "vulkan_device.h"

#include "core/logger.h"
#include "core/kstring.h"
#include "core/kmemory.h"

#include "containers/darray.h"

b8 vulkan_dev_create(vulkan_context *context) {
    if (!select_physical_dev(context))
        return FALSE;

    KINFO("Creating logical device...");
    //dont create additional queues for shared indices
    b8 present_shares_graphics_queue = context->dev.graphics_queue_index == context->dev.present_queue_index;
    b8 transfer_shares_graphics_queue = context->dev.graphics_queue_index == context->dev.transfer_queue_index;
    u32 ind_count = 1;
    if (!present_shares_graphics_queue)
        ++ind_count;
    if (!transfer_shares_graphics_queue)
        ++ind_count;
    u32 indices[ind_count];
    u8 ind = 0;
    indices[ind++] = context->dev.graphics_queue_index;
    if (!present_shares_graphics_queue)
        indices[ind++] = context->dev.present_queue_index;
    if (!transfer_shares_graphics_queue)
        indices[ind++] = context->dev.transfer_queue_index;

    VkDeviceQueueCreateInfo queue_create_infos[ind_count];
    for (u32 i = 0; i < ind_count; i++) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;

        //TODO: enable this for a future enhancement
        //if (indices[i] == context->dev.graphics_queue_index)
        //    queue_create_infos[i].queueCount = 2;

        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;

        f32 queue_priority = 1.0f;
        queue_create_infos[i].pQueuePriorities = &queue_priority;        
    }

    //req dev features
    //TODO: should be config driven
    VkPhysicalDeviceFeatures dev_features = {};
    dev_features.samplerAnisotropy = VK_TRUE;   //req anisotropy

    VkDeviceCreateInfo dev_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    dev_create_info.queueCreateInfoCount = ind_count;
    dev_create_info.pQueueCreateInfos = queue_create_infos;
    dev_create_info.pEnabledFeatures = &dev_features;
    dev_create_info.enabledExtensionCount = 1;
    
    const char *ext_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    dev_create_info.ppEnabledExtensionNames = &ext_names;

    //deprecated and ignored, so passing nothing
    dev_create_info.enabledLayerCount = 0;
    dev_create_info.ppEnabledLayerNames = 0;

    //create the dev
    VK_CHECK(vkCreateDevice(
        context->dev.physical_dev,
        &dev_create_info,
        context->allocator,
        &context->dev.logical_dev
    ));
    KINFO("Logical device created.");

    //get queues
    vkGetDeviceQueue(
        context->dev.logical_dev,
        context->dev.graphics_queue_index,
        0,
        &context->dev.graphics_queue
    );

    vkGetDeviceQueue(
        context->dev.logical_dev,
        context->dev.present_queue_index,
        0,
        &context->dev.present_queue
    );

    vkGetDeviceQueue(
        context->dev.logical_dev,
        context->dev.transfer_queue_index,
        0,
        &context->dev.transfer_queue
    );
    //TODO: for compute - dont we need it?
    KINFO("Queues obtained.");

    return TRUE;
}

void vulkan_dev_destroy(vulkan_context *context) {
    //wait till the dev is done
    vkDeviceWaitIdle(context->dev.logical_dev);

    KINFO("Destroying logical device...");
    if (context->dev.logical_dev) {
        vkDestroyDevice(context->dev.logical_dev, context->allocator);
        context->dev.logical_dev = 0;
    }

    //unset queues
    context->dev.graphics_queue = 0;
    context->dev.present_queue = 0;
    context->dev.transfer_queue = 0;

    KINFO("Releasing physical device resources...");
    context->dev.physical_dev = 0;

    if (context->dev.swapchain_support.formats) {
        kfree(context->dev.swapchain_support.formats,
            sizeof(VkSurfaceFormatKHR) * context->dev.swapchain_support.format_count,
            MEM_TAG_RENDERER);
        context->dev.swapchain_support.formats = 0;
        context->dev.swapchain_support.format_count = 0;
    }

    if (context->dev.swapchain_support.present_modes) {
        kfree(context->dev.swapchain_support.present_modes,
            sizeof(VkPresentModeKHR) * context->dev.swapchain_support.present_mode_count,
            MEM_TAG_RENDERER);
        context->dev.swapchain_support.present_modes = 0;
        context->dev.swapchain_support.present_mode_count = 0;
    }

    kzero_mem(&context->dev.swapchain_support.capabilities,
        sizeof(context->dev.swapchain_support.capabilities));

    context->dev.graphics_queue_index = -1;
    context->dev.present_queue_index = -1;
    context->dev.transfer_queue_index = -1;
}

b8 select_physical_dev(vulkan_context *context) {
    u32 physical_dev_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->inst, &physical_dev_count, 0));
    if (physical_dev_count == 0) {
        KFATAL("No devices with Vulkan support is found!");
        return FALSE;
    }

    VkPhysicalDevice physical_devs[physical_dev_count];
    VK_CHECK(vkEnumeratePhysicalDevices(context->inst, &physical_dev_count, physical_devs));

    for (u32 i = 0; i < physical_dev_count; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physical_devs[i], &props);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devs[i], &features);

        VkPhysicalDeviceMemoryProperties mem;
        vkGetPhysicalDeviceMemoryProperties(physical_devs[i], &mem);

        //TODO: these reqs probably should be driven by engine config
        vulkan_physical_device_requirements reqs = {};
        reqs.graphics = TRUE;
        reqs.present = TRUE;
        reqs.transfer = TRUE;
        //enable this if necessary
        //reqs.compute = TRUE;
        reqs.sampler_anisotropy = TRUE;
        reqs.discrete_gpu = TRUE;
        reqs.dev_ext_names = darray_create(const char *);
        darray_push(reqs.dev_ext_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 res = physical_dev_meets_reqs(
            physical_devs[i],
            context->surface,
            &props,
            &features,
            &reqs,
            &queue_info,
            &context->dev.swapchain_support
        );

        if (res) {
            KINFO("Selected device: '%s", props.deviceName);
            switch (props.deviceType) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    KINFO("GPU type is unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    KINFO("GPU type is integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    KINFO("GPU type is discrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    KINFO("GPU type is virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    KINFO("GPU type is CPU.");
                    break;
            }

            KINFO("GPU Driver Version: %d.%d.%d",
                VK_VERSION_MAJOR(props.driverVersion),
                VK_VERSION_MINOR(props.driverVersion),
                VK_VERSION_PATCH(props.driverVersion)
            );

            KINFO("VUlkan API Version: %d.%d.%d",
                VK_VERSION_MAJOR(props.apiVersion),
                VK_VERSION_MINOR(props.apiVersion),
                VK_VERSION_PATCH(props.apiVersion)
            );

            //mem info
            for (u32 j = 0; j < mem.memoryHeapCount; ++j) {
                f32 mem_size_gib = (((f32)mem.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if (mem.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    KINFO("Local GPU memory: %.2f GiB", mem_size_gib);
                else
                    KINFO("Shared system memory: %.2f GiB", mem_size_gib);
            }

            context->dev.physical_dev = physical_devs[i];
            context->dev.graphics_queue_index = queue_info.graphics_family_index;
            context->dev.present_queue_index = queue_info.present_family_index;
            context->dev.transfer_queue_index = queue_info.transfer_family_index;
            //set compute ind here if necessary

            //keep a copy of props, features and mem info for later use
            context->dev.props = props;
            context->dev.features = features;
            context->dev.memory = mem;

            break;
        }
    }

    //ensure a dev is selected
    if (!context->dev.physical_dev) {
        KERROR("No physical devices were found which meet the requirements!");
        return FALSE;
    }

    KINFO("Physical device selected.");
    return TRUE;
}

b8 physical_dev_meets_reqs(
    VkPhysicalDevice dev,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties *props,
    const VkPhysicalDeviceFeatures *features,
    const vulkan_physical_device_requirements *reqs,
    vulkan_physical_device_queue_family_info *out_queue_fam_info,
    vulkan_swapchain_support_info *out_swapchain_supp
) {
    //eval dev props to determine if it meets the needs of app
    out_queue_fam_info->graphics_family_index = -1;
    out_queue_fam_info->present_family_index = -1;
    out_queue_fam_info->compute_family_index = -1;
    out_queue_fam_info->transfer_family_index = -1;

    if (reqs->discrete_gpu) {
        if (props->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            KINFO("Device is not a discrete GPU and one is required, skipping device.");
            return FALSE;
        }
    }

    u32 queue_fam_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_fam_count, 0);
    VkQueueFamilyProperties queue_fams[queue_fam_count];
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_fam_count, queue_fams);

    //look at each queue to see what queues it supports
    KINFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_fam_count; i++) {
        u8 curr_transfer_score = 0;

        //graphics queue?
        if (queue_fams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_fam_info->graphics_family_index = i;
            ++curr_transfer_score;
        }

        //compute queue?
        if (queue_fams[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_fam_info->compute_family_index = i;
            ++curr_transfer_score;
        }

        //transfer queue? - specifically wanting to use transfer dedicated queues
        if (queue_fams[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            //take the ind if it is the current lowest,
            // this increases the likelyhood of being a dedicated transfer queue
            if (curr_transfer_score <= min_transfer_score) {
                min_transfer_score = curr_transfer_score;
                out_queue_fam_info->transfer_family_index = i;
            }
        }

        //present queue?
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &supports_present));
        if (supports_present) 
            out_queue_fam_info->present_family_index = i;
    }

    KINFO("       %d |       %d |       %d |        %d | %s",
            out_queue_fam_info->graphics_family_index != -1,
            out_queue_fam_info->present_family_index != -1,
            out_queue_fam_info->compute_family_index != -1,
            out_queue_fam_info->transfer_family_index != -1,
            props->deviceName);
    
    //TODO: what the heck??
    if ((!reqs->graphics || out_queue_fam_info->graphics_family_index != -1) &&
        (!reqs->present || out_queue_fam_info->present_family_index != -1) &&
        (!reqs->compute || out_queue_fam_info->compute_family_index != -1) &&
        (!reqs->transfer || out_queue_fam_info->transfer_family_index != -1)) {
            KINFO("Device meets queue requirements.");
            KTRACE("Graphics Family Index: %i", out_queue_fam_info->graphics_family_index);
            KTRACE("Present Family Index: %i", out_queue_fam_info->present_family_index);
            KTRACE("Transfer Family Index: %i", out_queue_fam_info->transfer_family_index);
            KTRACE("Compute Family Index: %i", out_queue_fam_info->compute_family_index);
            
            //query swapchain support
            vulkan_dev_query_swapchain_supp(dev, surface, out_swapchain_supp);

            if (out_swapchain_supp->format_count < 1 || out_swapchain_supp->present_mode_count < 1) {
                if (out_swapchain_supp->formats)
                    kfree(out_swapchain_supp->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_supp->format_count, MEM_TAG_RENDERER);
                if (out_swapchain_supp->present_modes)
                    kfree(out_swapchain_supp->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_supp->present_mode_count, MEM_TAG_RENDERER);
                KINFO("Required swapchain support not present, skipping device.");
                return FALSE;    
            }

            //dev exts
            if (reqs->dev_ext_names) {
                u32 available_ext_count = 0;
                VkExtensionProperties *available_exts = 0;
                VK_CHECK(vkEnumerateDeviceExtensionProperties(dev, 0, &available_ext_count, 0));

                if (available_ext_count != 0) {
                    available_exts = kallocate(sizeof(VkExtensionProperties) * available_ext_count, MEM_TAG_RENDERER);
                    VK_CHECK(vkEnumerateDeviceExtensionProperties(dev, 0, &available_ext_count, available_exts));
                    
                    u32 req_ext_count = darray_length(reqs->dev_ext_names);
                    for (u32 i = 0; i < req_ext_count; i++) {
                        b8 found = FALSE;
                        for (u32 j = 0; j < available_ext_count; j++) {
                            if (strs_equal(reqs->dev_ext_names[i], available_exts[j].extensionName)) {
                                found = TRUE;
                                break;
                            }
                        }

                        if (!found) {
                            KINFO("Required extension '%s' not found, skipping device.", reqs->dev_ext_names[i]);
                            kfree(available_exts, sizeof(VkExtensionProperties) * available_ext_count, MEM_TAG_RENDERER);
                            return FALSE;
                        }
                    } 
                }

                kfree(available_exts, sizeof(VkExtensionProperties) * available_ext_count, MEM_TAG_RENDERER);   
            }

            //sampler anisotropy
            if (reqs->sampler_anisotropy && !features->samplerAnisotropy) {
                KINFO("Device does not support samplerAnisotropy, skipping device.");
                return FALSE;
            }

            return TRUE;
    }
    
    return FALSE;
}

void vulkan_dev_query_swapchain_supp(
    VkPhysicalDevice physical_dev,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info *out_supp_info
) {
    //surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_dev,
        surface,
        &out_supp_info->capabilities
    ));

    //surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_dev,
        surface,
        &out_supp_info->format_count,
        0
    ));

    if (out_supp_info->format_count != 0) {
        if (!out_supp_info->formats)
            out_supp_info->formats = kallocate(sizeof(VkSurfaceFormatKHR) * out_supp_info->format_count, MEM_TAG_RENDERER);
        
            VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                physical_dev,
                surface,
                &out_supp_info->format_count,
                out_supp_info->formats
            ));
    }

    //present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_dev,
        surface,
        &out_supp_info->present_mode_count,
        0
    ));

    if (out_supp_info->present_mode_count != 0) {
        if (!out_supp_info->present_modes)
            out_supp_info->present_modes = kallocate(sizeof(VkPresentModeKHR) * out_supp_info->present_mode_count, MEM_TAG_RENDERER);
        
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_dev,
            surface,
            &out_supp_info->present_mode_count,
            out_supp_info->present_modes
        ));
    }
}

b8 vulkan_dev_detect_depth_format(vulkan_device *dev) {
    //format candidates
    const u64 candidate_count = 3;
    VkFormat candidates[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,        
    };

    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < candidate_count; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(dev->physical_dev, candidates[i], &props);
        
        if ((props.linearTilingFeatures & flags) == flags) {
            dev->depth_format = candidates[i];
            return TRUE;
        }

        if ((props.optimalTilingFeatures & flags) == flags) {
            dev->depth_format = candidates[i];
            return TRUE;
        }
    }

    return FALSE;
}