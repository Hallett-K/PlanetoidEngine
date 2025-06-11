#pragma once

#include <Planetoid/PlanetoidCore.h>
#include <Planetoid/Core/Delegate.h>

#include <string>
#include <unordered_map>

namespace PlanetoidEngine
{
    DECLARE_DELEGATE(OnSceneEnterDelegate);

    class Scene;
    class PE_API SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        bool CreateScene(const std::string& name, bool setCurrent = false);
        void DestroyScene(const std::string& name);
        void SetCurrentScene(const std::string& name);
        void InterruptLoad() { m_interrupt = true; }

        bool AddScene(const std::string& name, Scene* scene, bool setCurrent = false);

        Scene* GetCurrentScene() const;
        std::string GetCurrentSceneName() const;
        Scene* GetScene(const std::string& name) const;

        void Update(float deltaTime);

        bool IsSceneCreated(const std::string& name) const
        {
            return m_scenes.find(name) != m_scenes.end();
        }

        OnSceneEnterDelegate OnSceneEnter;

    private:
        std::unordered_map<std::string, Scene*> m_scenes;
        std::pair<std::string, Scene*> m_currentScene = { "", nullptr };
        bool m_interrupt = false;
    };
}