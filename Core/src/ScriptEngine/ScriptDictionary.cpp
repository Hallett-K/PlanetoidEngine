#include <Planetoid/ScriptEngine/ScriptDictionary.h>

namespace PlanetoidEngine
{
    ScriptDictionary::ScriptDictionary()
    {
    }

    ScriptDictionary::~ScriptDictionary()
    {
    }

    void ScriptDictionary::RemoveScript(const std::string& name)
    {
        m_scripts.erase(name);
    }

    IScript* ScriptDictionary::GetScript(const std::string& name) const
    {
        auto it = m_scripts.find(name);
        if (it != m_scripts.end())
        {
            return it->second();
        }
        return nullptr;
    }

    std::vector<std::string> ScriptDictionary::GetScriptNames() const
    {
        std::vector<std::string> scripts;
        for (auto& script : m_scripts)
        {
            scripts.push_back(script.first);
        }
        return scripts;
    }
}