#pragma once

#include <Planetoid/Core/Application.h>
#include <Planetoid/Core/EntryPoint.h>
#include <Planetoid/Core/FileIO.h>
#include <Planetoid/Core/InputManager.h>
#include <Planetoid/Core/UUID.h>

#include <Planetoid/Scenes/Scene.h>
#include <Planetoid/Scenes/Entity.h>
#include <Planetoid/Scenes/Components.h>

#include <Planetoid/ScriptEngine/ScriptComponent.h>

namespace PlanetoidEngine
{
    const unsigned int majorVersion = 0;
    const unsigned int minorVersion = 1;
    const unsigned int patchVersion = 0;

    std::string GetVersionString()
    {
        return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(patchVersion);
    }
}