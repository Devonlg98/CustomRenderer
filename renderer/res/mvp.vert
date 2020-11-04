#version 430
// -------- VERTEX ATTRIBUTES -------- 
layout (location = 0) in vec4 position;     
layout (location = 1) in vec4 color;        
layout (location = 2) in vec2 uv;           

// -------- UNIFORMS -------- 
layout (location = 0) uniform mat4 proj;    // proj
layout (location = 1) uniform mat4 view;    // view
layout (location = 2) uniform mat4 model;   // model

layout (location = 4) uniform float time;   

// --------- SENDS ------------
out vec4 vColor;    
out vec2 vUV;       

void main()
{ 
	vec4 finalPosition = position;

	gl_Position =  proj * view * model * finalPosition;
	vColor = color;
	vUV = uv;
};