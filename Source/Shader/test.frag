#version 430 core

layout(location=0) in vec4 pixelColor;
layout(location=0) out vec4 color;

void main()
{
	color = pixelColor;
}