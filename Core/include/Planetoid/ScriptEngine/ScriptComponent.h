#pragma once

#include <string>

#include <Planetoid/Scenes/Entity.h>

namespace PlanetoidEngine
{
    class PE_API IScript
    {
    public:
        IScript() = default;
        virtual ~IScript() = default;

        virtual void OnAttach(Entity e) = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
    };

    struct ScriptComponent
    {
        std::string scriptPath;
        IScript* script;
        bool isAttached;
    };
}