#include <Planetoid/ScriptEngine/ScriptAPI.h>

#include <Planetoid/Core/Application.h>

namespace PlanetoidEngine
{
    void ScriptAPI::GotoScene(const std::string& sceneName)
    {
        Application::GetInstance()->sceneManager.SetCurrentScene(sceneName);
    }

    std::string ScriptAPI::GetCurrentSceneName()
    {
        return Application::GetInstance()->sceneManager.GetCurrentSceneName();
    }

    Entity ScriptAPI::CreateEntity(const std::string& name)
    {
        return Application::GetInstance()->sceneManager.GetCurrentScene()->CreateEntity(name);
    }

    void ScriptAPI::DestroyEntity(Entity entity)
    {
        Application::GetInstance()->sceneManager.GetCurrentScene()->DestroyEntity(entity);
    }

    Entity ScriptAPI::GetEntityByUUID(const std::string& uuid)
    {
        PlanetoidEngine::UUID id = PlanetoidEngine::UUID(uuid);
        return Application::GetInstance()->sceneManager.GetCurrentScene()->GetEntity(id);
    }

    Entity ScriptAPI::GetEntityByUUID(const UUID& uuid)
    {
        return Application::GetInstance()->sceneManager.GetCurrentScene()->GetEntity(uuid);
    }

    Entity ScriptAPI::GetEntityByName(const std::string& name)
    {
        return Application::GetInstance()->sceneManager.GetCurrentScene()->GetEntity(name);
    }
}