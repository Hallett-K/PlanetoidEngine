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
    const unsigned int majorVersion = 2025;
    const unsigned int minorVersion = 6;
    const unsigned int patchVersion = 22;

    std::string GetVersionString()
    {
        // Format: YYYY-MM-DD
        std::string minorVersionString = std::to_string(minorVersion);
        if (minorVersion < 10)
            minorVersionString = "0" + minorVersionString;
        
        std::string patchVersionString = std::to_string(patchVersion);
        if (patchVersion < 10)
            patchVersionString = "0" + patchVersionString;
        
        return std::to_string(majorVersion) + "-" + minorVersionString + "-" + patchVersionString;
    }
}