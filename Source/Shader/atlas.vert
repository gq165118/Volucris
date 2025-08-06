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