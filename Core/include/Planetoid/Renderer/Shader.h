#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <string>

namespace PlanetoidEngine
{
    class PE_API Shader
    {
    public:
        Shader();
        ~Shader();

        void LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
        void LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

        void Bind() const;
        void Unbind() const;

        inline int GetHandle() const { return m_handle; }
    private:
        int m_handle;
    };
}