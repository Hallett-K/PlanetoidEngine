#include <Planetoid/Renderer/Renderer.h>

#include <Planetoid/Core/Log.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace PlanetoidEngine
{
    Renderer::Renderer()
    {
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::Init(unsigned int maxInstanceCount)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        m_maxInstanceCount = maxInstanceCount;
        m_instanceCount = 0;
        m_textureCount = 0;

        m_shader = new Shader();
        m_shader->LoadFromFile("assets/shaders/main.vert", "assets/shaders/main.frag");
        m_shader->Bind();

        m_vertices = {
            { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f }, { 1.0f, 0.0f } },
            { {  0.5f,  0.5f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f }, { 0.0f, 1.0f } }
        };

        m_indices = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

        glGenBuffers(1, &m_instanceDataBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceDataBuffer);
        GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(InstanceData) * m_maxInstanceCount, nullptr, flags);
        //m_instanceData = (InstanceData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(InstanceData) * m_maxInstanceCount, flags);

        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(InstanceData), (const void*)offsetof(InstanceData, id));
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)offsetof(InstanceData, depth));
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)offsetof(InstanceData, color));
        glVertexAttribDivisor(4, 1);

        for (int i = 0; i < 4; i++)
        {
            glEnableVertexAttribArray(5 + i);
            glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(offsetof(InstanceData, transform) + sizeof(glm::vec4) * i));
            glVertexAttribDivisor(5 + i, 1);
        }

        glGenBuffers(1, &m_indirectBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);
        glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), nullptr, flags);
        m_command = (DrawElementsIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand), flags);

        glGenBuffers(1, &m_textureBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_textureBuffer);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * m_maxInstanceCount, nullptr, flags);
        //m_textures = (GLuint64*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint64) * m_maxInstanceCount, flags);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_textureBuffer);

        m_command[0].vertexCount = m_indices.size();
        m_command[0].instanceCount = 0;
        m_command[0].firstIndex = 0;
        m_command[0].baseVertex = 0;
        m_command[0].baseInstance = 0;

        glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        m_viewMatrix = viewMatrix;
        m_projectionMatrix = projectionMatrix;
        m_instanceCount = 0;
    }

    void Renderer::Submit(Transform& transform, const glm::vec4& color, Texture* texture)
    {
        if (m_instanceCount >= m_maxInstanceCount)
        {
            Flush();
        }

        GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        m_instanceData = (InstanceData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(InstanceData) * m_maxInstanceCount, flags);
        m_command = (DrawElementsIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand), flags);
        m_textures = (GLuint64*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint64) * m_maxInstanceCount, flags);

        bool found = false;
        unsigned int textureIndex = 0;
        for (int i = 0; i < m_textureCount; i++)
        {
            if (m_textures[i] == texture->GetBindlessHandle())
            {
                textureIndex = i;
                found = true;
                break;
            }
        }

        if (!found)
        {
            m_textures[m_textureCount] = texture->GetBindlessHandle();
            textureIndex = m_textureCount;
            m_textureCount++;
        }

        m_instanceData[m_instanceCount].id = textureIndex;
        m_instanceData[m_instanceCount].depth = transform.GetPosition().z;
        m_instanceData[m_instanceCount].color = color;
        m_instanceData[m_instanceCount].transform = transform.GetMatrix();

        m_instanceCount++;
        m_command[0].instanceCount = m_instanceCount;

        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    void Renderer::Flush(bool clear)
    {
        if (clear)
        {
            glViewport(0, 0, 1280, 720);
            glClearColor(m_clearColour.r, m_clearColour.g, m_clearColour.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        if (m_instanceCount == 0)
        {
            return;
        }

        m_shader->Bind();

        for (int i = 0; i < m_textureCount; i++)
        {
            glMakeTextureHandleResidentARB(m_textures[i]);
        }

        glUniformMatrix4fv(glGetUniformLocation(m_shader->GetHandle(), "uView"), 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(m_shader->GetHandle(), "uProjection"), 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 1, 0);

        for (int i = 0; i < m_textureCount; i++)
        {
            glMakeTextureHandleNonResidentARB(m_textures[i]);
        }

        m_instanceCount = 0;
        m_textureCount = 0;
    }
}