#pragma once

#include <Planetoid/PlanetoidCore.h>
#include <Planetoid/Scenes/Scene.h>
#include <entt/entt.hpp>

namespace PlanetoidEngine
{
    class PE_API Entity
    {
    public:
        Entity();
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other);

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
        }

        template <typename T>
        T& GetComponent()
        {
            return m_scene->m_registry.get<T>(m_handle);
        }

        template <typename T>
        bool HasComponent()
        {
            return m_scene->m_registry.all_of<T>(m_handle);
        }

        template <typename T>
        void RemoveComponent()
        {
            m_scene->m_registry.remove<T>(m_handle);
        }

        operator entt::entity() const { return m_handle; }

        inline bool operator==(const Entity& other) const
        {
            return m_handle == other.m_handle && m_scene == other.m_scene;
        }

        inline bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }
    private:
        entt::entity m_handle;
        Scene* m_scene;
    };
}