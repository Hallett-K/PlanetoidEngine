#include <Planetoid/Renderer/Texture.h>
#include <Planetoid/Core/Extensions.h>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <time.h>

namespace PlanetoidEngine
{
    bool Texture::IsBindlessTextureSupported()
    {
        return Extensions::IsExtensionSupported("GL_ARB_bindless_texture");
    }

    Texture::Texture()
    {
        m_IsBindless = false;
    }

    Texture::~Texture()
    {
        
    }

    void Texture::LoadFromFile(const std::string& path, bool forceNoBindless)
    {
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        m_Path = path;
        m_SourceType = SourceType::File;

        LoadFromMemory(data, width, height, channels, forceNoBindless);

        stbi_image_free(data);
    }

    void Texture::LoadFromMemory(void* data, int width, int height, int channels, bool forceNoBindless)
    {
        if (m_IsCreated)
        {
            glDeleteTextures(1, &m_Handle);
        }

        GLenum format = GL_RGBA;
        switch (channels)
        {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureStorage2D(m_Handle, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(m_Handle, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

        Width = width;
        Height = height;
        Channels = channels;

        if (IsBindlessTextureSupported() && !forceNoBindless)
        {
            m_IsBindless = true;
            m_BindlessHandle = glGetTextureHandleARB(m_Handle);
        }
        else
        {
            m_IsBindless = false;
        }

        m_IsCreated = true;
    }

    void Texture::FramebufferColor(int width, int height, bool forceNoBindless)
    {
        if (m_IsCreated)
        {
            glDeleteTextures(1, &m_Handle);
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureStorage2D(m_Handle, 1, GL_RGBA8, width, height);

        Width = width;
        Height = height;
        Channels = 4;

        if (IsBindlessTextureSupported() && !forceNoBindless)
        {
            m_IsBindless = true;
            m_BindlessHandle = glGetTextureHandleARB(m_Handle);
        }
        else
        {
            m_IsBindless = false;
        }

        m_IsCreated = true;
    }

    void Texture::FramebufferDepthStencil(int width, int height, bool forceNoBindless)
    {
        if (m_IsCreated)
        {
            glDeleteTextures(1, &m_Handle);
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureStorage2D(m_Handle, 1, GL_DEPTH24_STENCIL8, width, height);

        Width = width;
        Height = height;
        Channels = 1;

        if (IsBindlessTextureSupported() && !forceNoBindless)
        {
            m_IsBindless = true;
            m_BindlessHandle = glGetTextureHandleARB(m_Handle);
        }
        else
        {
            m_IsBindless = false;
        }

        m_IsCreated = true;
    }

    void Texture::RandomNoise(int width, int height, int channels, bool forceNoBindless)
    {
        m_SourceType = SourceType::RandomNoise;

        unsigned char* data = new unsigned char[width * height * channels];
        for (int i = 0; i < width * height * channels; i++)
        {
            data[i] = rand() % 255;
            if (channels == 4 && i % 4 == 3)
            {
                data[i + 1] = 255;
            }
        }

        LoadFromMemory(data, width, height, channels, forceNoBindless);
    }

    void Texture::White(int width, int height, int channels, bool forceNoBindless)
    {
        m_SourceType = SourceType::Blank;
        
        unsigned char* data = new unsigned char[width * height * channels];
        for (int i = 0; i < width * height * channels; i++)
        {
            data[i] = 255;
        }

        LoadFromMemory(data, width, height, channels, forceNoBindless);
    }

    void Texture::Reserve(int width, int height, int channels, bool forceNoBindless)
    {
        LoadFromMemory(nullptr, width, height, channels, forceNoBindless);
    }

    void Texture::Bind(unsigned int slot) const
    {
        if (m_IsBindless)
        {
            glMakeTextureHandleResidentARB(m_BindlessHandle);
        }
        else
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, m_Handle);
        }
    }

    void Texture::Unbind() const
    {
        if (m_IsBindless)
        {
            glMakeTextureHandleNonResidentARB(m_BindlessHandle);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}