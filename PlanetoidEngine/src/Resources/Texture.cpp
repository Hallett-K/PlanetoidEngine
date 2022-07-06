#include "Texture.h"

#include <glad/glad.h>

namespace PlanetoidEngine
{
    void Texture::Bind(int slot)
    {
        glBindTexture(GL_TEXTURE_2D, handle);
        glBindTexture(GL_TEXTURE0 + slot, handle);
    }
}