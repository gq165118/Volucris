#version 430 core

layout(location=0) in vec4 pixelColor;
layout(location=1) in vec2 uv;

layout(location=0) out vec4 color;

uniform sampler2D u_texture;

uniform sampler2D u_texture2;

void main()
{
	vec4 faceColor = texture2D(u_texture2, uv);
	color = texture(u_texture, uv) * (1.0-faceColor.a) + faceColor * faceColor.a;
}