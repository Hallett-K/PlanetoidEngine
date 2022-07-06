#pragma once

#include <unordered_map>

#include "Resources/Shader.h"
#include "Resources/Texture.h"

namespace PlanetoidEngine
{
    class AssetManager
    {
    public:
        AssetManager();
        ~AssetManager();

        Texture* GetTexture(const char* path);
        bool RemoveTexture(const char* path);

        Shader* GetShader(const char* name);
        bool RemoveShader(const char* name);
    private:
        std::unordered_map<const char*, Texture*> m_textures;
        std::unordered_map<const char*, Shader*> m_shaders;
    };
}