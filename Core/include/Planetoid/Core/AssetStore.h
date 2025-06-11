#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <string>
#include <unordered_map>

namespace PlanetoidEngine
{
    enum class AssetType
    {
        None,
        Texture,
        Shader,
        Utility
    };

    struct Asset
    {
        AssetType type;
        void* data;
    };

    class PE_API AssetStore
    {
    public:
        AssetStore();
        ~AssetStore();

        Asset* GetOrCreateAsset(const std::string& path);
        void UnloadAsset(const std::string& path);

        bool IsAssetLoaded(const std::string& path) const;
    private:
        std::unordered_map<std::string, Asset> m_assets;
    };
}