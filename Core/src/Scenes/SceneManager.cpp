#include <Planetoid/Scenes/SceneManager.h>
#include <Planetoid/Scenes/Scene.h>

#include <vector>

namespace PlanetoidEngine
{
    SceneManager::SceneManager()
    {
        m_currentScene = std::make_pair("", nullptr);
    }

    SceneManager::~SceneManager()
    {
        // Create a copy of the scenes before iterating
        std::vector<std::pair<std::string, Scene*>> scenesCopy(m_scenes.begin(), m_scenes.end());

        // Clear the original map
        m_scenes.clear();

        // Delete scenes from the copy
        for (const auto& scene : scenesCopy)
        {
            delete scene.second;
        }
    }

    bool SceneManager::CreateScene(const std::string& name, bool setCurrent)
    {
        // Check if scene already exists
        if (m_scenes.find(name) != m_scenes.end() || name.empty())
        {
            return false;
        }

        // Create the scene
        Scene* scene = new Scene();

        // Add the scene to the map
        m_scenes.insert(std::make_pair(name, scene));

        // Set the scene as the current scene if requested
        if (setCurrent)
        {
            SetCurrentScene(name);
        }

        return true;
    }

    void SceneManager::DestroyScene(const std::string& name)
    {
        // Check if scene exists
        if (m_scenes.find(name) == m_scenes.end() || name.empty())
        {
            return;
        }

        if (m_currentScene.first == name)
        {
            m_currentScene = std::make_pair("", nullptr);
        }

        // Delete the scene
        delete m_scenes[name];

        // Remove the scene from the map
        m_scenes.erase(name);
    }

    void SceneManager::SetCurrentScene(const std::string& name)
    {
        // Check if scene exists
        if (m_scenes.find(name) == m_scenes.end() || name.empty())
        {
            return;
        }

        // Unload the current scene
        if (m_scenes.find(m_currentScene.first) != m_scenes.end() && m_currentScene.second != nullptr)
        {
            m_currentScene.second->OnUnload();
        }

        // Set the new current scene
        m_currentScene = std::make_pair(name, m_scenes[name]);

        OnSceneEnter.Broadcast();

        // Load the new current scene
        if (!m_interrupt)
            m_currentScene.second->OnLoad();

        m_interrupt = false;
    }

    bool SceneManager::AddScene(const std::string& name, Scene* scene, bool setCurrent)
    {
        // Check if scene already exists
        if (m_scenes.find(name) != m_scenes.end() || name.empty())
        {
            return false;
        }

        // Add the scene to the map
        m_scenes.insert(std::make_pair(name, scene));

        // Set the scene as the current scene if requested
        if (setCurrent)
        {
            SetCurrentScene(name);
        }

        return true;
    }

    Scene* SceneManager::GetCurrentScene() const
    {
        return m_currentScene.second;
    }

    std::string SceneManager::GetCurrentSceneName() const
    {
        if (m_currentScene.second == nullptr)
            return "";
        return m_currentScene.first;
    }

    Scene* SceneManager::GetScene(const std::string& name) const
    {
        // Check if scene exists
        if (m_scenes.find(name) == m_scenes.end() || name.empty())
        {
            return nullptr;
        }

        return m_scenes.at(name);
    }

    void SceneManager::Update(float deltaTime)
    {
        // Check if there is a current scene
        if (m_currentScene.second == nullptr)
        {
            return;
        }

        // Update the current scene
        if (m_currentScene.second->IsPaused() == false)
            return;
        m_currentScene.second->OnUpdate(deltaTime);
    }
}