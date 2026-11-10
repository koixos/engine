#include "vulkan_backend.h"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"
#include "vulkan_command_buffer.h"
#include "vulkan_framebuffer.h"
#include "vulkan_fence.h"
#include "vulkan_utils.h"

#include "core/logger.h"
#include "core/kstring.h"
#include "core/kmemory.h"
#include "core/application.h"

#include "containers/darray.h"

//static Vulkan context
static vulkan_context context;
static u32 cached_framebuf_width = 0;
static u32 cached_framebuf_height = 0;

i32 find_mem_ind(u32 type_filter, u32 property_flags);

void create_cmd_bufs(renderer_backend *backend);

void regenerate_framebufs(renderer_backend *backend, vulkan_swapchain *swapch, vulkan_renderpass *rp);

b8 recreate_swapchain(renderer_backend *backend);

b8 vulkan_renderer_backend_init(
    renderer_backend *backend,
    const char *app_name,
    struct platform_state *plat_st
) {
    //func ptrs
    context.find_mem_ind = find_mem_ind;

    //TODO: custom allocator
    context.allocator = 0;

    app_get_framebuf_size(&cached_framebuf_width, &cached_framebuf_height);
    context.framebuffer_width = (cached_framebuf_width != 0) ? cached_framebuf_width : 800;
    context.framebuffer_height = (cached_framebuf_height != 0) ? cached_framebuf_height : 600;
    cached_framebuf_width = 0;
    cached_framebuf_height = 0;

    //setup vulkan instance
    VkApplicationInfo app_info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No-Name Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;

    //obtain a list of required exts
    const char **req_exts = darray_create(const char *);
    darray_push(req_exts, &VK_KHR_SURFACE_EXTENSION_NAME);  //generic surface ext
    platform_get_required_ext_names(&req_exts);             //os-specific exts
#if defined(_DEBUG)
    darray_push(req_exts, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  //debug utilities

    KDEBUG("Required extensions: ");
    u32 len = darray_length(req_exts);
    for (u32 i = 0; i < len; i++)
        KDEBUG(req_exts[i]);
#endif

    create_info.enabledExtensionCount = darray_length(req_exts);
    create_info.ppEnabledExtensionNames = req_exts;

    //setting up validation layers
    const char **req_val_layer_names = 0;
    u32 req_val_layer_count = 0;

//if validation should be done, get a list of req.ed val-layer names
// and make sure they exist, val layers should only be enabled on non-release builds
#if defined(_DEBUG)
    KDEBUG("Validation layers enabled. Enumerating...");

    //list of val-layers req.ed
    req_val_layer_names = darray_create(const char *);
    darray_push(req_val_layer_names, &"VK_LAYER_KHRONOS_validation");
    req_val_layer_count = darray_length(req_val_layer_names);

    //obtain a list of available val-layers
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    VkLayerProperties *available_layers = darray_reserve(VkLayerProperties, available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

    //verify all req.ed layers are available
    for (u32 i = 0; i < req_val_layer_count; i++) {
        KDEBUG("Searching for layer: %s...", req_val_layer_names[i]);
        b8 found = FALSE;
        for (u32 j = 0; j < available_layer_count; j++) {
            if (strs_equal(req_val_layer_names[i], available_layers[j].layerName)) {
                found = TRUE;
                KDEBUG("Found.");
                break;
            }
        }
        
        if (!found) {
            KFATAL("Required validation layer '%s' is missing!", req_val_layer_names[i]);
            return FALSE;
        }
    }

    KDEBUG("All required validation layers are present.");
#endif

    create_info.enabledLayerCount = req_val_layer_count;
    create_info.ppEnabledLayerNames = req_val_layer_names;

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.inst));
    KINFO("Vulkan instance created.");

//debugger
#if defined(_DEBUG)
    KDEBUG("Creating Vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT; //|
                        //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
    
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info =
        { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;

    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            context.inst, "vkCreateDebugUtilsMessengerEXT");
    KASSERT_MSG(func, "Failed to create debug messenger!");
    VK_CHECK(func(context.inst, &debug_create_info, context.allocator, &context.debug_messenger));
    KDEBUG("Vulkan debugger created.");
#endif

    //surface
    KDEBUG("Creating Vulkan surface...");
    if (!platform_create_vulkan_surface(plat_st, &context)) {
        KERROR("Failed to create platform surface!");
        return FALSE;
    }
    KINFO("Vulkan surface created.");

    //device creation
    if (!vulkan_dev_create(&context)) {
        KERROR("Failed to create the device!");
        return FALSE;
    }

    //create swapchain
    vulkan_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain
    );

    //create render pass
    vulkan_renderpass_create(
        &context,
        &context.main_rp,
        0, 0, context.framebuffer_width, context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0
    );

    //swapchain 
    context.swapchain.framebufs = darray_reserve(vulkan_framebuffer, context.swapchain.img_count);
    regenerate_framebufs(backend, &context.swapchain, &context.main_rp);

    //create cmd bufs
    create_cmd_bufs(backend);

    //create sync objs
    context.img_available_sems = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.in_flight_fences = darray_reserve(vulkan_fence, context.swapchain.max_frames_in_flight);    
    context.queue_complete_sems = darray_reserve(VkSemaphore, context.swapchain.img_count); 

    VkSemaphoreCreateInfo sem_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; i++) {
        vkCreateSemaphore(context.dev.logical_dev, &sem_create_info, context.allocator, &context.img_available_sems[i]);
        
        //create the fence in a signaled st, indicating that the first frame has already been rendered
        // this will prevent the app from waiting indefinitely for first frame to render since
        // it cannot be rendered until a frame is rendered before it
        vulkan_fence_create(&context, TRUE, &context.in_flight_fences[i]);
    }

    for (u32 i = 0; i < context.swapchain.img_count; i++)
        vkCreateSemaphore(context.dev.logical_dev, &sem_create_info, context.allocator, &context.queue_complete_sems[i]);

    //in-flight fences should not yet exist at this point, thus the list should be cleared.
    // these are stored in ptrs because the init st should be 0 and will be 0 when not in use.
    // actual fences are not owned by this list
    context.imgs_in_flight = darray_reserve(vulkan_fence, context.swapchain.img_count);
    for (u32 i = 0; i < context.swapchain.img_count; i++)
        context.imgs_in_flight[i] = 0;

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend *backend) {
    //make surethat there is no graphical op is going on
    vkDeviceWaitIdle(context.dev.logical_dev);

    //destroy in the opposite order of creation

    KDEBUG("Destroying Vulkan synchronization objects...");
    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; i++) {
        if (context.img_available_sems[i]) {
            vkDestroySemaphore(
                context.dev.logical_dev,
                context.img_available_sems[i],
                context.allocator
            );
            context.img_available_sems[i] = 0;
        }

        vulkan_fence_destroy(&context, &context.in_flight_fences[i]);
    }

    for (u32 i = 0; i < context.swapchain.img_count; i++) {
        if (context.queue_complete_sems[i]) {
            vkDestroySemaphore(
                context.dev.logical_dev,
                context.queue_complete_sems[i],
                context.allocator
            );
            context.queue_complete_sems[i] = 0;
        }
    }

    darray_destroy(context.img_available_sems);
    context.img_available_sems = 0;

    darray_destroy(context.queue_complete_sems);
    context.queue_complete_sems = 0;

    darray_destroy(context.in_flight_fences);
    context.in_flight_fences = 0;

    darray_destroy(context.imgs_in_flight);
    context.imgs_in_flight = 0;
    
    KDEBUG("Destroying Vulkan command buffers...");
    for (u32 i = 0; i < context.swapchain.img_count; i++) {
        if (context.graphics_cmd_bufs[i].handle) {
            vulkan_cmd_buf_free(
                &context,
                context.dev.graphics_cmd_pool,
                &context.graphics_cmd_bufs[i]
            );
            context.graphics_cmd_bufs[i].handle = 0;
        }
    }        
    darray_destroy(context.graphics_cmd_bufs);
    context.graphics_cmd_bufs = 0;
    
    KDEBUG("Destroying Vulkan framebuffers...");
    for (u32 i = 0; i < context.swapchain.img_count; i++)
        vulkan_framebuf_destroy(&context, &context.swapchain.framebufs[i]);

    KDEBUG("Destroying Vulkan renderpass...");
    vulkan_renderpass_destroy(&context, &context.main_rp);

    KDEBUG("Destroying Vulkan swapchain...");
    vulkan_swapchain_destroy(&context, &context.swapchain);
    
    KDEBUG("Destroying Vulkan device...");
    vulkan_dev_destroy(&context);

    KDEBUG("Destroying Vulkan surface...");
    if (context.surface) {
        vkDestroySurfaceKHR(context.inst, context.surface, context.allocator);
        context.surface = 0;
    }

#if defined(_DEBUG)
    KDEBUG("Destroying Vulkan debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                context.inst, "vkDestroyDebugUtilsMessengerEXT");
        func(context.inst, context.debug_messenger, context.allocator);
    }
#endif

    KDEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.inst, context.allocator);
}

void vulkan_renderer_backend_on_resized(renderer_backend *backend, u16 width, u16 height) {
    cached_framebuf_width = width;
    cached_framebuf_height = height;

    //updt framebuf_size_generation that indicaets when the framebuffer size has been updated
    context.framebuf_size_generation++;

    KINFO("Vulkan renderer backend resized -- w/h/gen: %i/%i/%llu",
        width, height, context.framebuf_size_generation);
}

b8 vulkan_renderer_backend_begin_frame(renderer_backend *backend, f32 delta_time) {
    vulkan_device *dev = &context.dev;

    //check if recreating swapch and boot out
    if (context.recreating_swch) {
        VkResult res = vkDeviceWaitIdle(dev->logical_dev);
        if (!vulkan_result_is_success(res)) {
            KERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed: '%s'",
                vulkan_result_string(res, TRUE));
            return FALSE;
        }

        KINFO("Recreating swapchain, booting...");
        return FALSE;
    }

    //check if the framebuf has been resized; if so, a new swapch must be created
    if (context.framebuf_size_generation != context.framebuf_size_last_generation) {
        VkResult res = vkDeviceWaitIdle(dev->logical_dev);
        if (!vulkan_result_is_success(res)) {
            KERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed: '%s'",
                vulkan_result_string(res, TRUE));
            return FALSE;
        }
        
        //if the swapch recreation is failed, boot out before unsetting the flag
        if (!recreate_swapchain(backend))
            return FALSE;
        
        KINFO("Resized, booting...");
        return FALSE;
    }

    //wait for the execution of curr frame to complete, fence being free will allow this one to move on
    if (!vulkan_fence_wait(
        &context,
        &context.in_flight_fences[context.curr_frame],
        UINT64_MAX
    )) {
        KWARN("In-flight fence wait failure.");
        return FALSE;
    }

    //acquire the next img from the swapch, pass along the sem that should be signaled when
    // this is done. Same sem will later be waited on by the queue submission to ensure
    // that the img is available
    if (!vulkan_swapchain_acquire_next_img_ind(
        &context,
        &context.swapchain,
        UINT64_MAX,
        context.img_available_sems[context.curr_frame],
        0,
        &context.img_ind
    ))
        return FALSE;

    //begin recording cmds
    vulkan_command_buffer *cmd_buf = &context.graphics_cmd_bufs[context.img_ind];
    vulkan_cmd_buf_reset(cmd_buf);
    vulkan_cmd_buf_begin(cmd_buf, FALSE, FALSE, FALSE);

    //dynamic st
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32)context.framebuffer_height;
    viewport.width = (f32)context.framebuffer_width;
    viewport.height = -(f32)context.framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    //scissor
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context.framebuffer_width;
    scissor.extent.height = context.framebuffer_height;

    vkCmdSetViewport(cmd_buf->handle, 0, 1, &viewport);
    vkCmdSetScissor(cmd_buf->handle, 0, 1, &scissor);

    context.main_rp.w = context.framebuffer_width;    
    context.main_rp.h = context.framebuffer_height;

    //begin the render pass
    vulkan_renderpass_begin(
        cmd_buf,
        &context.main_rp,
        context.swapchain.framebufs[context.img_ind].handle
    );

    return TRUE;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend *backend, f32 delta_time) {
    vulkan_command_buffer *cmd_buf = &context.graphics_cmd_bufs[context.img_ind];

    //end render pass
    vulkan_renderpass_end(cmd_buf, &context.main_rp);
    
    vulkan_cmd_buf_end(cmd_buf);

    //make sure the prev frame not using this img, i.e. it is fence is not in use
    if (context.imgs_in_flight[context.img_ind] != VK_NULL_HANDLE)
        vulkan_fence_wait(
            &context,
            context.imgs_in_flight[context.img_ind],
            UINT64_MAX
        );

    //mark the img fence as in-use by this frame
    context.imgs_in_flight[context.img_ind] = &context.in_flight_fences[context.curr_frame];

    //reset the fence for use on the next frame
    vulkan_fence_reset(&context, &context.in_flight_fences[context.curr_frame]);

    //begin queue submission and wait for the op to complete
    VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

    //cmd buf(s) to be executed
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf->handle;

    //sem(s) to be signaled when queue is done
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &context.queue_complete_sems[context.img_ind];

    //wait-sem(s) ensures that the op cannot begin until the img is available
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &context.img_available_sems[context.curr_frame];

    //each sem waits on the corresponding pipeline stage to complete, 1:1 ratio.
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent clr attachment writes from
    //  executing until the sem signals, i.e. one frame is presented at a time
    VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.pWaitDstStageMask = flags;

    VK_CHECK(vkQueueSubmit(
        context.dev.graphics_queue,
        1,
        &submit_info,
        context.in_flight_fences[context.curr_frame].handle
    ));

    vulkan_cmd_buf_update_submitted(cmd_buf);
    //end of the queue submission op

    //give the img back to the swapchain
    vulkan_swapchain_present(
        &context,
        &context.swapchain,
        context.dev.graphics_queue,
        context.dev.present_queue,
        context.queue_complete_sems[context.img_ind],
        context.img_ind
    );

    return TRUE;
}

VkBool32 vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_types,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *usr_data
) {
    switch (msg_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            KERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            KWARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            KINFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            KTRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}

i32 find_mem_ind(u32 type_filter, u32 property_flags) {
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(context.dev.physical_dev, &mem_props);

    for (u32 i = 0; i < mem_props.memoryTypeCount; i++)
        //check each mem type to see if its bit is set to 1
        if (type_filter & (1 << i) && (mem_props.memoryTypes[i].propertyFlags & property_flags) == property_flags)
            return i;

    KWARN("Unable to find a suitable memory type.");
    return -1;
}

void create_cmd_bufs(renderer_backend *backend) {
    if (!context.graphics_cmd_bufs) {
        context.graphics_cmd_bufs = darray_reserve(vulkan_command_buffer, context.swapchain.img_count);
        for (u32 i = 0; i < context.swapchain.img_count; i++)
            kzero_mem(&context.graphics_cmd_bufs[i], sizeof(vulkan_command_buffer));
    }

    for (u32 i = 0; i < context.swapchain.img_count; i++) {
        if (context.graphics_cmd_bufs[i].handle)
            vulkan_cmd_buf_free(
                &context,
                context.dev.graphics_cmd_pool,
                &context.graphics_cmd_bufs[i]
            );
        
        kzero_mem(&context.graphics_cmd_bufs[i], sizeof(vulkan_command_buffer));
        vulkan_cmd_buf_allocate(
            &context,
            context.dev.graphics_cmd_pool,
            TRUE,
            &context.graphics_cmd_bufs[i]    
        );
    }

    KINFO("Vulkan command buffers created.");
}

void regenerate_framebufs(renderer_backend *backend, vulkan_swapchain *swapch, vulkan_renderpass *rp) {
    for (u32 i = 0; i < swapch->img_count; i++) {
        //TODO: make this dynamic based on the currently configured attachments
        u32 attachment_count = 2;
        VkImageView attachments[] = {
            swapch->views[i],
            swapch->depth_attachment.view
        };

        vulkan_framebuf_create(
            &context,
            rp,
            context.framebuffer_width,
            context.framebuffer_height,
            attachment_count,
            attachments,
            &context.swapchain.framebufs[i]
        );
    }
}

b8 recreate_swapchain(renderer_backend *backend) {
    //if already being recreated, dont try again
    if (context.recreating_swch) {
        KDEBUG("recreate_swapchain called when already recreating, booting...");
        return FALSE;
    }

    //detect if the window is too small to be drawn to
    if (context.framebuffer_width == 0 || context.framebuffer_height == 0) {
        KDEBUG("recreate_swapchain called when window is < 1 in a dimension, booting...");
        return FALSE;
    }

    //mark as recreating
    context.recreating_swch = TRUE;

    //wait for any ops to complete
    vkDeviceWaitIdle(context.dev.logical_dev);

    //clear imgs-in-flight out just in case
    for (u32 i = 0; i < context.swapchain.img_count; i++)
        context.imgs_in_flight[i] = 0;

    //requery supp
    vulkan_dev_query_swapchain_supp(
        context.dev.physical_dev,
        context.surface,
        &context.dev.swapchain_support
    );

    vulkan_dev_detect_depth_format(&context.dev);

    vulkan_swapchain_recreate(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain
    );

    //sync the framebuf size with the cached ones
    context.framebuffer_width = context.main_rp.w = cached_framebuf_width;
    context.framebuffer_height = context.main_rp.h = cached_framebuf_height;
    cached_framebuf_width = 0;
    cached_framebuf_height = 0;

    //updt framebuf size gen
    context.framebuf_size_last_generation = context.framebuf_size_generation;

    //cleanup swapch
    for (u32 i = 0; i < context.swapchain.img_count; i++)
        vulkan_cmd_buf_free(&context, context.dev.graphics_cmd_pool, &context.graphics_cmd_bufs[i]);
    
    //framebufs
    for (u32 i = 0; i < context.swapchain.img_count; i++)
        vulkan_framebuf_destroy(&context, &context.swapchain.framebufs[i]);

    context.main_rp.x = 0;
    context.main_rp.y = 0;
    context.main_rp.w = context.framebuffer_width;
    context.main_rp.h = context.framebuffer_height;

    regenerate_framebufs(backend, &context.swapchain, &context.main_rp);

    create_cmd_bufs(backend);

    //clear the recreating flag
    context.recreating_swch = FALSE;

    return TRUE;
}