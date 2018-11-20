#version 310 es

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec3 vertex_colour;


layout(std140, push_constant) uniform param_block 
{
	mat4 model;
	mat4 view_projection;
} params;

layout(location = 0) out mediump vec4 color;

void main()
{	

    vec3 light_pos = vec3(-3.f, 3.f, -5.f);
	vec3 world_light = (params.model * vec4(light_pos, 1.0f)).xyz; // use light pos
	vec4 world_pos = params.model * vec4(vertex_position, 1.0f);
	vec3 world_normal = (params.model * vec4(vertex_normal, 0.0f)).xyz;
	
	vec3 light_dir = world_light - world_pos.xyz;
	float brightness = dot(light_dir, world_normal) / length(light_dir) / length(world_normal);
	brightness = abs(brightness);
	
    gl_Position = params.view_projection * world_pos;
    vec3 col = vertex_colour * brightness;
    color = vec4(col, 1.f);
}

