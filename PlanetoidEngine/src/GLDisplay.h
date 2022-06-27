#pragma once


// Forward Declarations
struct GLFWwindow;

namespace PlanetoidEngine
{
    class GLDisplay
    {
    public:
        GLDisplay();
        ~GLDisplay();

        bool Init(unsigned int width, unsigned int height, const char* title);

        void Update();

        void CleanUp();

        // TODO: Make callbacks private
        void framebuffer_resize(unsigned int width, unsigned int height);
    private:
        GLFWwindow* m_windowHandle;
    };
}