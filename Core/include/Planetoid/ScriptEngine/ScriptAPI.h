#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Core/FileIO.h>
#include <Planetoid/Core/InputManager.h>
#include <Planetoid/Core/Log.h>
#include <Planetoid/Scenes/Entity.h>
#include <Planetoid/Scenes/Components.h>

namespace PlanetoidEngine
{
    class PE_API ScriptAPI
    {
    public:

        // Scenes
        static void GotoScene(const std::string& sceneName);
        static std::string GetCurrentSceneName();

        // Entities
        static Entity CreateEntity(const std::string& name = "");
        static void DestroyEntity(Entity entity);
        static Entity GetEntityByUUID(const std::string& uuid);
        static Entity GetEntityByUUID(const UUID& uuid);
        static Entity GetEntityByName(const std::string& name);
    };
}