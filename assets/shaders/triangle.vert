#version 310 es

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 vertex_colour;

/*layout(std140, set = 0, binding = 0) uniform per_frame_param_block 
{
	mat4 view_projection;
	mat4 model;
} frame_parameters;*/


layout(std140, push_constant) uniform param_block 
{
	mat4 model;
	mat4 view_projection;
} params;

layout(location = 0) out mediump vec4 fragment_colour;

void main()
{	
    gl_Position = params.view_projection * params.model * vertex_position;
    fragment_colour = vertex_colour;
}
