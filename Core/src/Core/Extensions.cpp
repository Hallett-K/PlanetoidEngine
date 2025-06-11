#include <Planetoid/Core/Extensions.h>

#include <glad/glad.h>

namespace PlanetoidEngine
{
    std::set<std::string> Extensions::s_SupportedExtensions;
    unsigned int Extensions::s_ExtensionCount = 0;
    
    bool Extensions::IsExtensionSupported(const std::string& extension)
    {
        if (s_SupportedExtensions.size() == 0 && s_ExtensionCount == 0)
        {
            // Get the number of extensions supported by the driver
            glGetIntegerv(GL_NUM_EXTENSIONS, (GLint*)&s_ExtensionCount);

            // Get the supported extensions and add them to the set
            for (unsigned int i = 0; i < s_ExtensionCount; i++)
            {
                const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
                s_SupportedExtensions.insert(ext);
            }
        }

        return s_SupportedExtensions.find(extension) != s_SupportedExtensions.end();
    }
}