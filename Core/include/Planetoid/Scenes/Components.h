#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Planetoid/Renderer/Texture.h>

namespace PlanetoidEngine
{
    struct NameComponent
    {
        std::string name;

        NameComponent() = default;
        NameComponent(const NameComponent&) = default;
        NameComponent(const std::string& name)
            : name(name) {}
    };

    struct Transform
    {
    public:
        Transform()
            : m_position(0.0f), m_rotation(0.0f), m_scale(1.0f), m_dirty(true), m_cachedMatrix(1.0f) {}
        Transform(const Transform&) = default;
        Transform(const glm::vec3& position, float rotation, const glm::vec2& scale)
            : m_position(position), m_rotation(rotation), m_scale(scale), m_dirty(true), m_cachedMatrix(1.0f) {}

        inline const glm::vec3& GetPosition() const { return m_position; }
        inline void SetPosition(const glm::vec3& position) { m_position = position; m_dirty = true; }
        inline void SetPosition(const glm::vec2& position) { m_position.x = position.x; m_position.y = position.y; m_dirty = true; }

        inline float GetRotation() const { return m_rotation; }
        inline void SetRotation(float rotation) { m_rotation = rotation; m_dirty = true; }

        inline const glm::vec2& GetScale() const { return m_scale; }
        inline void SetScale(const glm::vec2& scale) { m_scale = scale; m_dirty = true; }

        inline const glm::mat4& GetMatrix()
        {
            if (m_dirty)
            {
                m_cachedMatrix = glm::translate(glm::mat4(1.0f), m_position);
                m_cachedMatrix = glm::rotate(m_cachedMatrix, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, -1.0f));
                m_cachedMatrix = glm::scale(m_cachedMatrix, glm::vec3(m_scale, 1.0f));
                m_dirty = false;
            }
            return m_cachedMatrix;
        }

        void Translate(const glm::vec3& translation) { m_position += translation; m_dirty = true; }
        void Translate(const glm::vec2& translation) { m_position.x += translation.x; m_position.y += translation.y; m_dirty = true; }
        void Rotate(float rotation) { m_rotation += rotation; m_dirty = true; }
        void Scale(const glm::vec2& scale) { m_scale *= scale; m_dirty = true; }
        void Scale(float scale) { m_scale *= scale; m_dirty = true; }
    private:
        glm::vec3 m_position;
        float m_rotation;
        glm::vec2 m_scale;

        bool m_dirty;
        glm::mat4 m_cachedMatrix;
    };

    struct SpriteRenderer
    {
        glm::vec4 color = glm::vec4(1.0f);
        Texture* texture;

        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;
        SpriteRenderer(const glm::vec4& color, Texture* texture)
            : color(color), texture(texture) {}
    };

    struct Camera
    {
        // View matrix is just TRS
        glm::mat4 projection;

        float left = 0.0f;
        float right = 0.0f;
        float bottom = 0.0f;
        float top = 0.0f;
        float nearPlane = -1.0f;
        float farPlane = 1.0f;

        Camera() = default;
        Camera(const Camera&) = default;
        Camera(const glm::mat4& projection)
            : projection(projection) {}

        const glm::mat4& GetProjection() const { return projection; }
        void SetProjection(const glm::mat4& projection) { this->projection = projection; }
        void SetProjection(float left, float right, float bottom, float top, float nearPlane = -1.0f, float farPlane = 1.0f)
        {
            this->left = left;
            this->right = right;
            this->bottom = bottom;
            this->top = top;
            this->nearPlane = nearPlane;
            this->farPlane = farPlane;
            this->projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
        }
    };

    // Physics

    struct BoxCollider2D
    {
        glm::vec2 size = glm::vec2(0.5f);
        glm::vec2 offset = glm::vec2(0.0f);

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;

        bool scaleWithTransform = true;

        BoxCollider2D() = default;
        BoxCollider2D(const BoxCollider2D&) = default;
    };

    struct CircleCollider2D
    {
        float radius = 0.5f;
        glm::vec2 offset = glm::vec2(0.0f);

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;

        bool scaleWithTransform = true;

        CircleCollider2D() = default;
        CircleCollider2D(const CircleCollider2D&) = default;
    };

    struct RigidBody2D
    {
        enum class RigidBodyType
        {
            Static = 0,
            Dynamic,
            Kinematic
        };

        RigidBodyType type = RigidBodyType::Static;
        bool allowRotation = true;

        RigidBody2D() = default;
        RigidBody2D(const RigidBody2D&) = default;

        glm::vec2 GetPosition() const;
        void SetPosition(const glm::vec2& position);

        float GetRotation() const;
        void SetRotation(float rotation);

        void ApplyForce(const glm::vec2& force, const glm::vec2& point);
        void ApplyTorque(float torque);
        void ApplyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point);
        void ApplyAngularImpulse(float impulse);

        void* _body;
    };

    // Timeline

    struct TimelineComponent
    {

    };
}
