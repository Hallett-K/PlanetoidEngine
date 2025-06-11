#include <Planetoid/Core/AssetStore.h>

#include <Planetoid/Core/FileIO.h>
#include <Planetoid/Renderer/Texture.h>
#include <Planetoid/Renderer/Shader.h>
#include <Planetoid/ScriptEngine/ScriptComponent.h>

namespace PlanetoidEngine
{
    AssetStore::AssetStore()
    {
    }

    AssetStore::~AssetStore()
    {
    }

    Asset* AssetStore::GetOrCreateAsset(const std::string& path)
    {
        if (m_assets.find(path) != m_assets.end())
        {
            return &m_assets[path];
        }

        Asset asset;
        asset.type = AssetType::None;
        asset.data = nullptr;

        if (!PlanetoidEngine::FileIO::FileExists(path))
        {
            return nullptr;
        }

        // Get the file extension
        std::string extension = path.substr(path.find_last_of(".") + 1);
        if (extension == "png")
        {
            asset.type = AssetType::Texture;
            asset.data = new Texture();
            ((Texture*)asset.data)->LoadFromFile(path);
        }
        else if (extension == "vert")
        {
            std::string fragPath = path.substr(0, path.find_last_of(".")) + ".frag";
            if (!PlanetoidEngine::FileIO::FileExists(fragPath))
            {
                return nullptr;
            }

            asset.type = AssetType::Shader;
            asset.data = new Shader();
            ((Shader*)asset.data)->LoadFromFile(path, fragPath);
        }
        else if (extension == "frag")
        {
            std::string vertPath = path.substr(0, path.find_last_of(".")) + ".vert";
            if (!PlanetoidEngine::FileIO::FileExists(vertPath))
            {
                return nullptr;
            }

            asset.type = AssetType::Shader;
            asset.data = new Shader();
            ((Shader*)asset.data)->LoadFromFile(vertPath, path);
        }

        m_assets[path] = asset;
        return &m_assets[path];
    }

    void AssetStore::UnloadAsset(const std::string& path)
    {
        if (m_assets.find(path) != m_assets.end())
        {
            m_assets.erase(path);
        }
    }

    bool AssetStore::IsAssetLoaded(const std::string& path) const
    {
        return m_assets.find(path) != m_assets.end();
    }
}