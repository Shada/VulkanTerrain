#version 310 es

precision mediump float;

layout(location = 0) out vec4 result_colour;
layout(location = 0) in mediump vec4 fragment_colour;

void main()
{
    result_colour = fragment_colour;
}
