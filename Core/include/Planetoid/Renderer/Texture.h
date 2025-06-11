#pragma once

#include <string>

#include <Planetoid/PlanetoidCore.h>

namespace PlanetoidEngine
{
    class PE_API Texture
    {
    public:
        static bool IsBindlessTextureSupported();

        Texture();
        ~Texture();

        void LoadFromFile(const std::string& path, bool forceNoBindless = false);
        void LoadFromMemory(void* data, int width, int height, int channels, bool forceNoBindless = false);
        void FramebufferColor(int width, int height, bool forceNoBindless = false);
        void FramebufferDepthStencil(int width, int height, bool forceNoBindless = false);
        void RandomNoise(int width, int height, int channels, bool forceNoBindless = false);
        void White(int width, int height, int channels, bool forceNoBindless = false);
        void Reserve(int width, int height, int channels, bool forceNoBindless = false);

        void Bind(unsigned int slot = 0) const;
        void Unbind() const;

        inline unsigned int GetHandle() const { return m_Handle; }
        inline unsigned long long GetBindlessHandle() const { return m_BindlessHandle; }

        int Width;
        int Height;
        int Channels;

        bool operator==(const Texture& other) const
        {
            return (Width == other.Width && Height == other.Height && Channels == other.Channels && m_Handle == other.m_Handle && m_BindlessHandle == other.m_BindlessHandle && m_IsBindless == other.m_IsBindless);
        }

        enum class SourceType
        {
            Uninitialized,
            File,
            Blank,
            RandomNoise
        };

        SourceType GetSourceType() const { return m_SourceType; }

        const std::string& GetPath() const { return m_Path; }

    private:
        unsigned int m_Handle;
        unsigned long long m_BindlessHandle;
        char* m_Data;
        std::string m_Path;

        bool m_IsBindless;
        bool m_IsCreated;

        SourceType m_SourceType = SourceType::Uninitialized;
    };
}