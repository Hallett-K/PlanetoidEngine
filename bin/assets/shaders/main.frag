#version 460 core

#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec4 fColor;
in vec2 fTexCoords;
flat in uint fTextureID;

layout(binding = 0, std430) readonly buffer ssbo3 {
    sampler2D textures[];
};

void main() 
{
    sampler2D tex = textures[fTextureID];
    FragColor = texture(tex, fTexCoords) * fColor;
}