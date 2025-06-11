#include <Planetoid/Renderer/Shader.h>

#include <glad/glad.h>

#include <fstream>
#include <sstream>

namespace PlanetoidEngine
{
    Shader::Shader()
    {
        m_handle = glCreateProgram();
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_handle);
    }

    void Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexSource;
        std::string fragmentSource;

        std::ifstream vertexFile;
        std::ifstream fragmentFile;

        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vertexFile.open(vertexPath);
            fragmentFile.open(fragmentPath);

            std::stringstream vertexStream;
            std::stringstream fragmentStream;

            vertexStream << vertexFile.rdbuf();
            fragmentStream << fragmentFile.rdbuf();

            vertexFile.close();
            fragmentFile.close();

            vertexSource = vertexStream.str();
            fragmentSource = fragmentStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            printf("ERROR: Failed to read shader file\n");
        }

        LoadFromSource(vertexSource, fragmentSource);
    }

    void Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource)
    {
        unsigned int vertexShader;
        unsigned int fragmentShader;

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vertexSourceCStr = vertexSource.c_str();
        const char* fragmentSourceCStr = fragmentSource.c_str();

        glShaderSource(vertexShader, 1, &vertexSourceCStr, NULL);
        glShaderSource(fragmentShader, 1, &fragmentSourceCStr, NULL);

        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);

        int success;
        char infoLog[512];

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            printf("Error: Failed to compile vertex shader: %s\n", infoLog);
        }

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            printf("Error: Failed to compile fragment shader: %s\n", infoLog);
        }

        glAttachShader(m_handle, vertexShader);
        glAttachShader(m_handle, fragmentShader);

        glLinkProgram(m_handle);

        glGetProgramiv(m_handle, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(m_handle, 512, NULL, infoLog);
            printf("Error: Failed to link shader program: %s\n", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_handle);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }
}