#version 430 // minimum 430 for layout specifiers on uniforms

in vec2 vUV;    // from vertex shader
in vec3 vNormal;    // from vertex shader

out vec4 outputColor; // for frame buffer

layout (location = 3) uniform sampler2D mainTexture; // material 
layout (location = 5) uniform vec3 ambient; // environmental ambient
layout (location = 6) uniform vec3 lightDiffuse; // light diffuse
layout (location = 7) uniform vec3 lightDirection; // light direction

void main()
{
    vec3 ambientColor = ambient;

    float lambert = max ( 0.0f, dot(vNormal, -lightDirection));

    vec3 diffuseColor = texture(mainTexture, vUV).xyz * lightDiffuse * lambert;

    outputColor = vec4(ambientColor + diffuseColor, 1.0f);
}