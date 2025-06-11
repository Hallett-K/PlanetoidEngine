#include <Planetoid/Renderer/Framebuffer.h>

#include <glad/glad.h>

namespace PlanetoidEngine
{
    Framebuffer::Framebuffer()
    {
        m_fbo = 0;
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_fbo);
    }

    void Framebuffer::Init(int width, int height)
    {
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        m_ColorAttachment.FramebufferColor(width, height);
        m_DepthAttachment.FramebufferDepthStencil(width, height);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment.GetHandle());

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment.GetHandle(), 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment.GetHandle(), 0);


        if (glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete!\n");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}