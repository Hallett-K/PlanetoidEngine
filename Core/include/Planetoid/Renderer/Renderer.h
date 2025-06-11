#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Renderer/Shader.h>
#include <Planetoid/Renderer/Texture.h>
#include <Planetoid/Scenes/Components.h>

//
#include <glm/glm.hpp>

#include <vector>

typedef unsigned int GLuint;
typedef uint64_t GLuint64;

namespace PlanetoidEngine
{
    class PE_API Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Init(unsigned int maxInstanceCount = 100);

        void BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        void Submit(Transform& transform, const glm::vec4& color, Texture* texture);
        void Flush(bool clear = true);

        void setClearColour(const glm::vec3& colour) { m_clearColour = colour; }
        void setClearColour(float r, float g, float b) { m_clearColour = glm::vec3(r, g, b); }
    private:

        unsigned int m_maxInstanceCount = 100;

        struct Vertex
        {
            glm::vec2 position;
            glm::vec2 texCoords;
        };

        struct InstanceData
        {
            unsigned int id;
            float depth;
            glm::vec4 color;
            glm::mat4 transform;
        };

        struct DrawElementsIndirectCommand
        {
            GLuint vertexCount;
            GLuint instanceCount;
            GLuint firstIndex;
            GLuint baseVertex;
            GLuint baseInstance;
        };

        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ibo;
        GLuint m_instanceDataBuffer;
        GLuint m_indirectBuffer;
        GLuint m_textureBuffer;

        Shader* m_shader;

        std::vector<Vertex> m_vertices;
        std::vector<GLuint> m_indices;
        InstanceData* m_instanceData;
        DrawElementsIndirectCommand* m_command;
        GLuint64* m_textures;

        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;

        unsigned int m_instanceCount;
        unsigned int m_drawCount;
        unsigned int m_textureCount;

        glm::vec3 m_clearColour;
    };
}