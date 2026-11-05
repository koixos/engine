#include "vulkan_backend.h"
#include "vulkan_platform.h"
#include "vulkan_device.h"

#include "core/logger.h"
#include "core/kstring.h"

#include "containers/darray.h"

//static Vulkan context
static vulkan_context context;

b8 vulkan_renderer_backend_init(
    renderer_backend *backend,
    const char *app_name,
    struct platform_state *plat_st
) {
    //TODO: custom allocator
    context.allocator = 0;

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

    KINFO("Vulkan renderer initialized successfully.");

    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend *backend) {
    //destroy in the opposite order of creation
    
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
    //TODO: fill
}

b8 vulkan_renderer_backend_begin_frame(renderer_backend *backend, f32 delta_time) {
    return TRUE;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend *backend, f32 delta_time) {
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