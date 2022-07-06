#pragma once

namespace PlanetoidEngine
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        bool AttachShaders(const char* vertexPath, const char* fragmentPath);

        void Use();

        void SetUniformTex(const char* uniformName, int textureSlot);

    private:
        unsigned int m_programID;
        bool m_linked;
    };
}