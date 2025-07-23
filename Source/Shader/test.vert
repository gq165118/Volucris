#version 430 core

layout(location=0) in vec3 v_pos;
layout(location=2) in vec4 v_color;

// u_color.group=base u_color.display_name=Color
// u_scale.group=Mesh u_scale.display_name=Scale
uniform vec4 u_color;
uniform float u_scale;

layout(location=0) out vec4 pixelColor;

void main()
{
	pixelColor = u_color;
	gl_Position = vec4(v_pos * u_scale, 1.0);
}