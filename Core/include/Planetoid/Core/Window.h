#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <string>

struct GLFWwindow;
namespace PlanetoidEngine
{
    enum ScreenMode
    {
        Windowed,
        Fullscreen,
        Borderless
    };

    struct WindowProperties
    {
        ScreenMode mode = ScreenMode::Windowed;
        std::string title = "Planetoid Engine";
        int width = 1280;
        int height = 720;
        bool startMaximised = false;

        void UpdateWindowProperties();
    };

    class PE_API Window
    {
    public:
        Window();
        ~Window();

        void Init(const WindowProperties& properties);
        void PollEvents();
        void SwapBuffers();
        void Shutdown();

        inline bool IsOpen() const { return m_Open; }

        inline int GetWidth() const { return m_Width; }
        inline int GetHeight() const { return m_Height; }

        void SetWindowTitle(const char* title);
        void SetWindowIcon(int width, int height, unsigned char* pixels);
        void SetWindowIcon(const std::string& path);

    private:
        bool m_Open = false;
        GLFWwindow* m_Handle = nullptr;

        int m_Width = 0;
        int m_Height = 0;
    };
}