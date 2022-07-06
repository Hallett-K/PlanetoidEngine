#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>

namespace PlanetoidEngine
{
    Shader::Shader()
        : m_linked(false)
    {
        m_programID = glCreateProgram();
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_programID);
    }

    bool Shader::AttachShaders(const char* vertexPath, const char* fragmentPath)
    {
        if (m_linked)
            return false;

        // Load Vertex Shader
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::ifstream vsf(vertexPath);
        std::ostringstream vss;
        vss << vsf.rdbuf();
        std::string vShaderSourceString = vss.str();
        const char* vShaderSrc = vShaderSourceString.c_str();
        vsf.close();

        // Compile Vertex Shader
        glShaderSource(vertexShader, 1, &vShaderSrc, NULL);
        glCompileShader(vertexShader);
        int  vSuccess;
        char vInfoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vSuccess);
        if (!vSuccess)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, vInfoLog);
            printf("Shader compilation failed\nShader file: %s\nInfo: %s\n", vertexPath, vInfoLog);
            return false;
        }

        // Load Fragment Shader
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::ifstream fsf(fragmentPath);
        std::ostringstream fss;
        fss << fsf.rdbuf();
        std::string fShaderSourceString = fss.str();
        const char* fShaderSrc = fShaderSourceString.c_str();

        glShaderSource(fragmentShader, 1, &fShaderSrc, NULL);
        glCompileShader(fragmentShader);

        int fSuccess;
        char fInfoLog[512];

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fSuccess);
        if (!fSuccess)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, fInfoLog);
            printf("Shader compilation failed\nShader file: %s\nInfo: %s\n", fragmentPath, fInfoLog);
            return false;
        }

        // Link
        glAttachShader(m_programID, vertexShader);
        glAttachShader(m_programID, fragmentShader);
        glLinkProgram(m_programID);

        // Check Link
        int lSuccess;
        char lInfoLog[512];
        glGetProgramiv(m_programID, GL_LINK_STATUS, &lSuccess);
        if (!lSuccess) 
        {
            glGetProgramInfoLog(m_programID, 512, NULL, lInfoLog);
            printf("Shader Link failed\nInfo: %s\n", lInfoLog);
            return false;
        }

        m_linked = true;
        return true;
    }

    void Shader::Use()
    {
        glUseProgram(m_programID);
    }

    void Shader::SetUniformTex(const char* uniformName, int textureSlot)
    {
        glUniform1i(glGetUniformLocation(m_programID, uniformName), textureSlot);
    }

}