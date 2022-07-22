#pragma once

#include <glm/mat4x4.hpp>

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
        void SetUniformMat4(const char* uniformName, const glm::mat4& matrix);

    private:
        unsigned int m_programID;
        bool m_linked;
    };
}