#include "AssetManager.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace PlanetoidEngine
{
    AssetManager::AssetManager()
    {

    }

    AssetManager::~AssetManager()
    {
        m_shaders.clear();
        m_textures.clear();
    }

    Texture* AssetManager::GetTexture(const char* path)
    {
        if (m_textures.count(path) != 0)
            return NULL;

        int width, height, numChannels;
        unsigned char* texData = stbi_load(path, &width, &height, &numChannels, 0);
        if (!texData)
            return NULL;
        unsigned int texture;
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int format = (numChannels == 3 ? GL_RGB : GL_RGBA);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
        
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(texData);

        m_textures[path] = new Texture{ texture, width, height };
        return m_textures[path];
    }

    bool AssetManager::RemoveTexture(const char* path)
    {
        return m_textures.erase(path);
    }

    Shader* AssetManager::GetShader(const char* name)
    {
        if (m_shaders.count(name) != 0)
            return NULL;

        m_shaders[name] = new Shader;
        return m_shaders[name];
    }

    bool AssetManager::RemoveShader(const char* name)
    {
        return m_shaders.erase(name);
    }

}