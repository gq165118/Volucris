#version 430 core

layout(location=0) in vec3 v_pos;
layout(location=4) in vec2 v_uv;

uniform vec4 u_offset;  
uniform vec4 u_resolution; 
uniform vec4 u_quadSize;  

out vec2 v_texCoord;

void main() {

    vec2 position = u_offset.xy + v_pos.xy * u_quadSize.xy;
    
    vec2 ndc = (position.xy / u_resolution.xy) * 2.0 - 1.0;
    
    gl_Position = vec4(ndc.x, -ndc.y, 0.0, 1.0);
    
    v_texCoord = v_uv;
}

//#version 430 core
//
//layout(location=0) in vec3 v_pos;
//layout(location=2) in vec4 v_color;
//layout(location=4) in vec3 v_uv;
//
//// u_color.group=base u_color.display_name=Color u_color.value=vec4(1.0,0.0,0.0,1.0)
//// u_scale.group=Mesh u_scale.display_name=Scale u_scale.value=1.0
//uniform vec4 u_color;
//uniform float u_scale;
//
//layout(location=0) out vec4 pixelColor;
//layout(location=1) out vec2 uv;
//
//void main()
//{
//	pixelColor = u_color;
//	uv = v_uv.xy;
//	gl_Position = vec4(v_pos * u_scale, 1.0);
//}