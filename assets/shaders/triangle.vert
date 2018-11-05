#version 310 es

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 vertex_normal;
layout(location = 2) in vec4 vertex_colour;


layout(std140, push_constant) uniform param_block 
{
	mat4 model;
	mat4 view_projection;
} params;

layout(location = 0) out mediump vec4 color;

void main()
{	

    vec4 light_pos = vec4(-3.f, 3.f, -5.f, 1.0f);
	vec4 world_light = params.model * light_pos; // use light pos
	vec4 world_pos = params.model * vertex_position;
	vec4 world_normal = params.model * vertex_normal;
	
	vec4 light_dir = world_light - world_pos;
	float brightness = dot(light_dir, world_normal) / length(light_dir) / length(world_normal);
	brightness = abs(brightness);
	
    gl_Position = params.view_projection * world_pos;
    vec3 col = vertex_colour.xyz * brightness;
    color = vec4(col,1.f);
}

