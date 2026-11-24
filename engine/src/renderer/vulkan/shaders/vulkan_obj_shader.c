#include "vulkan_obj_shader.h"

#include "core/logger.h"
#include "core/kmemory.h"

#include "renderer/vulkan/vulkan_shader_utils.h"
#include "renderer/vulkan/vulkan_pipeline.h"
#include "renderer/vulkan/vulkan_buffer.h"

#include "math/math_types.h"

#define BUILTIN_SHADER_NAME_OBJ "Builtin_ObjShader" //must be same name within the post-build file

b8 vulkan_obj_shader_create(vulkan_context *context, vulkan_obj_shader *out_shader) {
    //shader module init per stage
    char stage_type_strs[OBJ_SHADER_STAGE_COUNT][5] = { "vert", "frag" };
    VkShaderStageFlagBits stage_types[OBJ_SHADER_STAGE_COUNT] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT
    };

    for (u32 i = 0; i < OBJ_SHADER_STAGE_COUNT; i++) {
        if (!create_shader_module(
                context, 
                BUILTIN_SHADER_NAME_OBJ, 
                stage_type_strs[i], 
                stage_types[i],
                i,
                out_shader->stages
        )) {
            KERROR("Unable to create %s shader module for '%s'.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJ);
            return false;
        }
    }

    //global descriptors
    VkDescriptorSetLayoutBinding global_ubo_layout_binding;
    global_ubo_layout_binding.binding = 0;
    global_ubo_layout_binding.descriptorCount = 1;
    global_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_ubo_layout_binding.pImmutableSamplers = 0;
    global_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo global_layout_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    global_layout_create_info.bindingCount = 1;
    global_layout_create_info.pBindings = &global_ubo_layout_binding;
    VK_CHECK(vkCreateDescriptorSetLayout(
        context->dev.logical_dev,
        &global_layout_create_info,
        context->allocator,
        &out_shader->global_desc_set_layout
    ));

    //global desc pool used for global items such as view/projection matrix
    VkDescriptorPoolSize global_pool_size;
    global_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_pool_size.descriptorCount = context->swapchain.img_count;    //also could write 3

    VkDescriptorPoolCreateInfo global_pool_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    global_pool_create_info.poolSizeCount = 1;
    global_pool_create_info.pPoolSizes = &global_pool_size;
    global_pool_create_info.maxSets = context->swapchain.img_count;

    VK_CHECK(vkCreateDescriptorPool(
        context->dev.logical_dev, 
        &global_pool_create_info, 
        context->allocator,
        &out_shader->global_desc_pool
    ));

    /* start of pipeline creation */
    VkViewport vp;
    vp.x = 0.0f;
    vp.y = (f32)context->framebuffer_height;
    vp.width = (f32)context->framebuffer_width;
    vp.height = -(f32)context->framebuffer_height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebuffer_width;
    scissor.extent.height = context->framebuffer_height;

    //attributes
    u32 offset = 0;
    const i32 attr_count = 1;
    VkVertexInputAttributeDescription attr_descs[1]; //[attr_count]
    
    //position
    VkFormat formats[1] = { //[attr_count]
        VK_FORMAT_R32G32B32_SFLOAT
    };

    u64 sizes[1] = { //[attr_count]
        sizeof(vec3)
    };

    for (u32 i = 0; i < attr_count; i++) {
        attr_descs[i].binding = 0;  //binding ind - should match binding desc
        attr_descs[i].location = i; //attr loc
        attr_descs[i].format = formats[i];
        attr_descs[i].offset = offset;
        offset += sizes[i];
    }

    //desc set layouts
    const i32 desc_set_layout_count = 1;
    VkDescriptorSetLayout layouts[1] = { out_shader->global_desc_set_layout };  //[desc_set_layout_count]

    //stages - should match the # of shader->stages
    VkPipelineShaderStageCreateInfo stage_create_infos[OBJ_SHADER_STAGE_COUNT];
    kzero_mem(stage_create_infos, sizeof(stage_create_infos));
    for (u32 i = 0; i < OBJ_SHADER_STAGE_COUNT; i++) {
        //stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }
    /* ----------------- */

    if (!vulkan_pipeline_create(
        context,
        &context->main_rp,
        attr_count,
        attr_descs,
        desc_set_layout_count,
        layouts,
        OBJ_SHADER_STAGE_COUNT,
        stage_create_infos,
        vp,
        scissor,
        false,
        &out_shader->pipeline
    )) {
        KERROR("Failed to load graphics pipeline for object shader.");
        return false;
    }

    //create uniform buf
    if (!vulkan_buffer_create(
        context,
        sizeof(global_uniform_obj) * 3,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true,
        &out_shader->global_uniform_buf
    )) {
        KERROR("Failed to create Vulkan buffer for object shader.");
        return false;
    }

    //alloc global desc sets
    VkDescriptorSetLayout global_layouts[3] = {
        out_shader->global_desc_set_layout,
        out_shader->global_desc_set_layout,
        out_shader->global_desc_set_layout
    };

    VkDescriptorSetAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.descriptorPool = out_shader->global_desc_pool;
    alloc_info.descriptorSetCount = 3;
    alloc_info.pSetLayouts = global_layouts;
    VK_CHECK(vkAllocateDescriptorSets(context->dev.logical_dev, &alloc_info, out_shader->global_desc_sets));

    return true;
}

void vulkan_obj_shader_destroy(vulkan_context *context, struct vulkan_obj_shader *shader) {
    VkDevice logical_dev = context->dev.logical_dev;

    //destroy uniform buf
    vulkan_buffer_destroy(context, &shader->global_uniform_buf);

    vulkan_pipeline_destroy(context, &shader->pipeline);
    
    //destroy global desc pool
    vkDestroyDescriptorPool(logical_dev, shader->global_desc_pool, context->allocator);

    //destroy global set layouts
    vkDestroyDescriptorSetLayout(logical_dev, shader->global_desc_set_layout, context->allocator);    

    //destroy shader modules
    for (u32 i = 0; i < OBJ_SHADER_STAGE_COUNT; i++) {
        vkDestroyShaderModule(context->dev.logical_dev, shader->stages[i].handle, context->allocator);
        shader->stages[i].handle = 0;
    }
}

void vulkan_obj_shader_use(vulkan_context *context, struct vulkan_obj_shader *shader) {
    u32 img_ind = context->img_ind;
    vulkan_pipeline_bind(&context->graphics_cmd_bufs[img_ind], VK_PIPELINE_BIND_POINT_GRAPHICS, &shader->pipeline);
}

void vulkan_obj_shader_update_global_st(vulkan_context *context, vulkan_obj_shader *shader) {
    u32 img_ind = context->img_ind;
    
    VkCommandBuffer cmd_buf = context->graphics_cmd_bufs[img_ind].handle;
    VkDescriptorSet glob_desc = shader->global_desc_sets[img_ind];

    if (!shader->desc_updated[img_ind]) {
        //config the descs for given ind
        u32 range = sizeof(global_uniform_obj);
        u64 offset = sizeof(global_uniform_obj) * img_ind;

        //copy data to buf
        vulkan_buffer_load_data(context, &shader->global_uniform_buf, offset, range, 0, &shader->global_ubo);

        VkDescriptorBufferInfo buf_info;
        buf_info.buffer = shader->global_uniform_buf.handle;
        buf_info.offset = offset;
        buf_info.range = range;

        //upd desc sets
        VkWriteDescriptorSet desc_write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        desc_write.dstSet = shader->global_desc_sets[img_ind];
        desc_write.dstBinding = 0;
        desc_write.dstArrayElement = 0;
        desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        desc_write.descriptorCount = 1;
        desc_write.pBufferInfo = &buf_info;

        vkUpdateDescriptorSets(context->dev.logical_dev, 1, &desc_write, 0, 0);
    }

    //bind the glob desc set to be updated
    vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->pipeline.pipeline_layout, 0, 1, &glob_desc, 0, 0);    
}