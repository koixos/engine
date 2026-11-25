#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;

//we have a SET of descriptors here (set=0), 
// and this is the first desc (binding=0) within that first set
layout(set = 0, binding = 0) uniform global_uniform_obj {
    mat4 projection;
    mat4 view;
} global_ubo;

layout(push_constant) uniform push_constants {
    //only quaranteed a total of 128 bytes
    mat4 model; //64 bytes
} u_push_consts;

void main() {
    gl_Position = global_ubo.projection * global_ubo.view * u_push_consts.model * vec4(in_position, 1.0);
}