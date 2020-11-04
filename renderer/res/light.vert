#version 430

// -------- VERTEX ATTRIBUTES -------- 
layout (location = 0) in vec4 position;     // in from vertex data
layout (location = 1) in vec4 color;        // in from vertex data
layout (location = 2) in vec2 uv;           // in from vertex data
layout (location = 3) in vec4 normal;       // in from vertex data

// -------- UNIFORMS -------- 
layout (location = 0) uniform mat4 proj;    // proj
layout (location = 1) uniform mat4 view;    // view
layout (location = 2) uniform mat4 model;   // model

layout (location = 4) uniform float time;   // time since application start

// --------- SENDS ------------
out vec2 vUV;
out vec3 vNormal;

void main()
{
    vec4 finalPosition = position;
    gl_Position = proj * view * model * position;
    
    vUV = uv;
    vNormal = (model * normal).xyz;
};