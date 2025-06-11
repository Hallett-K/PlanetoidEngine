#include <Planetoid/Scenes/Entity.h>

namespace PlanetoidEngine
{
    Entity::Entity()
    {
        m_handle = entt::null;
        m_scene = nullptr;
    }

    Entity::Entity(entt::entity handle, Scene* scene)
    {
        m_handle = handle;
        m_scene = scene;
    }

    Entity::Entity(const Entity& other)
    {
        m_handle = other.m_handle;
        m_scene = other.m_scene;
    }
}