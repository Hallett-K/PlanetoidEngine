#include <Planetoid/Core/Window.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Planetoid/Core/Application.h>

#include <stb/stb_image.h>

namespace PlanetoidEngine
{
    Window::Window()
    {

    }

    Window::~Window()
    {

    }

    void Window::Init(const WindowProperties& properties)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWmonitor* monitor = (properties.mode == ScreenMode::Windowed) ? nullptr : glfwGetPrimaryMonitor();

        if (properties.mode == ScreenMode::Borderless)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        }
        else if (properties.mode == ScreenMode::Windowed)
        {
            glfwWindowHint(GLFW_MAXIMIZED, properties.startMaximised);
        }

        m_Handle = glfwCreateWindow(properties.width, properties.height, properties.title.c_str(), monitor, nullptr);
        glfwMakeContextCurrent(m_Handle);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            // TODO: Log error
        }

        glViewport(0, 0, 1600, 900);

        glfwSetWindowUserPointer(m_Handle, this);
        glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window)
        {
            Window* instance = (Window*)glfwGetWindowUserPointer(window);
            instance->m_Open = false;
        });

        m_Open = true;

        // TODO: Callbacks
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Handle);
    }

    void Window::Shutdown()
    {
        glfwDestroyWindow(m_Handle);
        glfwTerminate();
    }

    void Window::SetWindowTitle(const char* title)
    {
        glfwSetWindowTitle(m_Handle, title);
    }

    void Window::SetWindowIcon(int width, int height, unsigned char* pixels)
    {
        GLFWimage image;
        image.width = width;
        image.height = height;
        image.pixels = pixels;

        glfwSetWindowIcon(m_Handle, 1, &image);
    }

    void Window::SetWindowIcon(const std::string& path)
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 0);

        SetWindowIcon(width, height, pixels);

        stbi_image_free(pixels);
    }
}