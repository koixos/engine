#pragma once

#include "defines.h"

#include "core/asserts.h"

#include "renderer/renderer_types.inl"

#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                  \
    {                                   \
        KASSERT(expr == VK_SUCCESS);    \
    }

#define OBJ_SHADER_STAGE_COUNT 2

typedef enum vulkan_renderpass_state {
    READY,
    RECORDING,
    IN_RENDERPASS,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_renderpass_state;

typedef enum vulkan_command_buffer_state {
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDERPASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;

typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR *formats;
    u32 present_mode_count;
    VkPresentModeKHR *present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_device {
    VkPhysicalDevice physical_dev;
    VkDevice logical_dev;
    vulkan_swapchain_support_info swapchain_support;
    
    i32 graphics_queue_fam_ind;
    i32 present_queue_fam_ind; 
    i32 transfer_queue_fam_ind;

    VkQueue graphics_queue;
    VkQueue present_queue; 
    VkQueue transfer_queue;

    VkCommandPool graphics_cmd_pool;

    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    VkFormat depth_format;
} vulkan_device;

typedef struct vulkan_renderpass {
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r, g, b, a;

    f32 depth;
    u32 stencil;

    vulkan_renderpass_state state;
} vulkan_renderpass;

typedef struct vulkan_framebuffer {
    VkFramebuffer handle;
    u32 attachment_count;
    VkImageView *attachments;
    vulkan_renderpass *renderpass;
} vulkan_framebuffer;

typedef struct vulkan_image {
    VkImage handle;
    VkDeviceMemory mem;
    VkImageView view;
    u32 width;
    u32 height;
} vulkan_image;

typedef struct vulkan_swapchain {
    VkSurfaceFormatKHR img_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 img_count;
    VkImage *imgs;
    VkImageView *views;
    vulkan_image depth_attachment;
    //framebuffers used for on-screen rendering
    vulkan_framebuffer *framebufs;
    VkExtent2D extent;
} vulkan_swapchain;

typedef struct vulkan_buffer {
    u64 total_size;
    VkBuffer handle;
    VkBufferUsageFlagBits usg;
    b8 is_locked;
    VkDeviceMemory mem;
    i32 mem_ind;
    u32 mem_prop_flags;
} vulkan_buffer;

typedef struct vulkan_command_buffer {
    VkCommandBuffer handle;
    //cmd buf state
    vulkan_command_buffer_state state;
} vulkan_command_buffer;

typedef struct vulkan_shader_stage {
    VkShaderModuleCreateInfo create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
} vulkan_shader_stage;

typedef struct vulkan_pipeline {
    VkPipeline handle;
    VkPipelineLayout pipeline_layout;
} vulkan_pipeline;

typedef struct vulkan_obj_shader {
    vulkan_shader_stage stages[OBJ_SHADER_STAGE_COUNT]; //vertex, fragment

    VkDescriptorPool global_desc_pool;
    VkDescriptorSetLayout global_desc_set_layout;
    VkDescriptorSet global_desc_sets[3];                //one desc set per frame - max 3 for triple-buffering
    b8 desc_updated[3];
    
    global_uniform_obj global_ubo;                      //global uniform obj
    vulkan_buffer global_uniform_buf;                   //global uniform buf

    vulkan_pipeline pipeline;
} vulkan_obj_shader;

typedef struct vulkan_fence {
    VkFence handle;
    b8 is_signaled;
} vulkan_fence;

typedef struct vulkan_context {
    VkInstance inst;
    VkAllocationCallbacks *allocator;
    VkSurfaceKHR surface;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif

    vulkan_device dev;

    u32 framebuffer_width;  //framebuffer's curr width
    u32 framebuffer_height; //framebuffer's curr height

    //curr generation of framebuf size; if this does not match framebuf_size_last_generation,
    // a new one shoulf be generated
    u64 framebuf_size_generation;

    //the generation of the framebuf when it was last created,
    // set to framebuf_size_generation when updated
    u64 framebuf_size_last_generation;

    vulkan_swapchain swapchain;
    u32 img_ind;
    u32 curr_frame;
    b8 recreating_swch;

    vulkan_renderpass main_rp;

    vulkan_command_buffer *graphics_cmd_bufs;   //darr

    vulkan_buffer obj_vert_buf;
    vulkan_buffer obj_ind_buf;
    
    vulkan_obj_shader obj_shader;

    u64 geometry_vert_offset;
    u64 geometry_ind_offset;

    VkSemaphore *img_available_sems;    //darr
    VkSemaphore *queue_complete_sems;   //darr

    u32 in_flight_fence_count;
    vulkan_fence *in_flight_fences;
    vulkan_fence **imgs_in_flight;      //holds ptrs to fences which exist and are owned elsewhere

    i32 (*find_mem_ind)(u32 type_filter, u32 property_flags);
} vulkan_context;
