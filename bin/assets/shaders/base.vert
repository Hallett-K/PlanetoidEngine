#version 460 core

#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in uint aTexID;
layout (location = 3) in vec4 aColor;
layout (location = 4) in mat4 aModel;

out vec4 fColor;
out vec2 fTexCoords;
flat out uint fTextureID;

void main() {
    gl_Position = aModel * vec4(aPos, 0.0, 1.0);
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTextureID = aTexID;
}