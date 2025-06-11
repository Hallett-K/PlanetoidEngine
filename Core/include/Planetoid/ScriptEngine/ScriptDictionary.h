#pragma once

#include <Planetoid/PlanetoidCore.h>
#include <Planetoid/ScriptEngine/ScriptComponent.h>

#include <string>
#include <unordered_map>

namespace PlanetoidEngine
{
    using ScriptFactory = IScript* (*)();
    class PE_API ScriptDictionary
    {
    public:
        ScriptDictionary();
        ~ScriptDictionary();

        template <typename T>
        void AddScript(const std::string& name)
        {
            static_assert(std::is_base_of<IScript, T>::value, "T must derive from IScript");
            m_scripts[name] = static_cast<ScriptFactory>([]() -> IScript* { return new T(); });
        }

        void RemoveScript(const std::string& name);

        IScript* GetScript(const std::string& name) const;
        std::vector<std::string> GetScriptNames() const;
    private:
        std::unordered_map<std::string, ScriptFactory> m_scripts;
    };
}

