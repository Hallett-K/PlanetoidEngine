#pragma once

#include <set>
#include <string>

namespace PlanetoidEngine
{
    class Extensions
    {
    public:
        static bool IsExtensionSupported(const std::string& extension);

    private:
        static std::set<std::string> s_SupportedExtensions;
        static unsigned int s_ExtensionCount;
    };
}