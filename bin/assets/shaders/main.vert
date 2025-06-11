#version 460 core

#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in uint aTexID;
layout (location = 3) in float depth;
layout (location = 4) in vec4 aColor;
layout (location = 5) in mat4 aModel;

out vec4 fColor;
out vec2 fTexCoords;
flat out uint fTextureID;

uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vec4 pos = uProjection * uView * aModel * vec4(aPos, 0.0, 1.0);
    pos.z = depth;
    gl_Position = pos;
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTextureID = aTexID;
}