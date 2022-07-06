#pragma once

namespace PlanetoidEngine
{
    struct Texture
    {
        unsigned int handle;
        int width;
        int height;

        void Bind(int slot);
    };
}