#include "vulkan_backend.h"

#include "core/logger.h"

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
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = 0;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult res = vkCreateInstance(&create_info, context.allocator, &context.inst);
    if (res != VK_SUCCESS) {
        KERROR("vkCreateInstance() failed with result: %u", res);
        return FALSE;
    }

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend *backend) {

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