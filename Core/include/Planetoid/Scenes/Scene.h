#pragma once

#include <Planetoid/PlanetoidCore.h>
#include <Planetoid/Core/UUID.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <unordered_map>

class b2World;
class b2Body;

namespace PlanetoidEngine
{
    class Entity;
    class Transform;
    class RigidBody2D;
    class PE_API Scene
    {
    public:
        Scene();
        ~Scene();

        void OnLoad();
        void OnUpdate(float deltaTime);
        void OnUnload();

        void SetPaused(bool paused);
        bool IsPaused() const;

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntity(const UUID& uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        Entity GetEntity(const std::string& name);
        Entity GetEntity(const UUID& uuid);

        std::vector<Entity> GetAllEntities();

        entt::registry& GetRegistry() { return m_registry; }

    private:
        friend class Entity;
        friend class Application;

        bool m_paused;
        entt::registry m_registry;

        std::unordered_map<UUID, Entity> m_uuidMap;
        std::unordered_map<UUID, b2Body*> m_physicsBodyMap;

        b2World* m_physicsWorld = nullptr;
        float m_pixelsPerMeter = 100.0f;
        glm::vec2 m_gravity = glm::vec2(0.0f, -15.0f);
        float m_restitutionThreshold = 0.2f;

        
        void CreatePhysicsEntity(Entity entity, const UUID& uuid, Transform& transform, RigidBody2D& rigidBody);
    };
}