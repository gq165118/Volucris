#version 430 core

layout(location=0) in vec3 v_pos;
layout(location=2) in vec4 v_color;
layout(location=4) in vec3 v_uv;

// u_color.group=base u_color.display_name=Color u_color.value=vec4(1.0,0.0,0.0,1.0)
// u_scale.group=Mesh u_scale.display_name=Scale u_scale.value=1.0
uniform vec4 u_color;
uniform float u_scale;

layout(location=0) out vec4 pixelColor;
layout(location=1) out vec2 uv;

void main()
{
	pixelColor = u_color;
	uv = v_uv.xy;
	gl_Position = vec4(v_pos * u_scale, 1.0);
}