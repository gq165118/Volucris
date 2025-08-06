#version 430 core

//! include "common.frag"

layout(location=0) in vec4 pixelColor;
layout(location=1) in vec2 uv;

layout(location=0) out vec4 color;


void main()
{
	color = pixelColor;
}