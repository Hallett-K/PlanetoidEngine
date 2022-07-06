#version 330 core

in vec2 fTex;
out vec4 col;

uniform sampler2D uTex;

void main()
{
    col = texture(uTex, fTex);
}