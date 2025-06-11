#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Renderer/Texture.h>

namespace PlanetoidEngine
{
    class PE_API Framebuffer
    {
    public:
        Framebuffer();
        ~Framebuffer();

        void Init(int width, int height);

        void Bind() const;
        void Unbind() const;

        inline unsigned int GetHandle() const { return m_fbo; }
        inline Texture& GetColorAttachment() { return m_ColorAttachment; }
        inline Texture& GetDepthAttachment() { return m_DepthAttachment; }
    private:
        unsigned int m_fbo, m_rbo;
        Texture m_ColorAttachment;
        Texture m_DepthAttachment;
    };
}