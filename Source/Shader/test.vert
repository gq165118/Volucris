#version 430 core

layout(location=0) in vec3 v_pos;
layout(location=2) in vec4 v_color;

uniform vec4 u_color;

layout(location=0) out vec4 pixelColor;

void main()
{
	gl_Position = vec4(v_pos * 10.f, 1.0);
}