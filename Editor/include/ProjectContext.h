#pragma once

#include <string>
#include <vector>
#include <windows.h>

#include <PlanetoidEngine.h>

struct ProjectContext
{
    std::string name;
    std::string rootPath;
    std::string startupSceneName;

    std::string currentSceneName;

    HINSTANCE dll = NULL;

    typedef void(*ScriptDLLAttachFunc)();
    ScriptDLLAttachFunc OnAttachFunc;

    typedef void(*ScriptDLLDetachFunc)();
    ScriptDLLDetachFunc OnDetachFunc;

    typedef PlanetoidEngine::IScript*(*GetScriptDLLFunc)(const std::string&);
    GetScriptDLLFunc GetScriptFunc;

    typedef std::vector<std::string>(*GetScriptNamesDLLFunc)();
    GetScriptNamesDLLFunc GetScriptNamesFunc;
};
