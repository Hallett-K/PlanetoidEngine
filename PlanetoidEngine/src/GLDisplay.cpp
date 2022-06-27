#include "GLDisplay.h"

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Static callback helpers
static void framebuffer_resize_s(GLFWwindow* window, int width, int height);

namespace PlanetoidEngine
{

    GLDisplay::GLDisplay()
        : m_windowHandle(NULL)
    {
    }

    GLDisplay::~GLDisplay()
    {
    }

    bool GLDisplay::Init(unsigned int width, unsigned int height, const char* title)
    {
        if (glfwInit() == GLFW_FALSE)
        {
            // TODO: Error handling
            // TODO: Logging system
            printf("ERROR: GLFW failed to initialise\n");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_windowHandle = glfwCreateWindow(width, height, title, NULL, NULL);
        if (m_windowHandle == NULL)
        {
            // TODO: Error handling
            // TODO: Logging system
            printf("ERROR: Failed to create GLFW window\n");
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(m_windowHandle);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            // TODO: Error handling
            // TODO: Logging system
            printf("ERROR: Failed to initialise GL Link Library\n");
            glfwTerminate();
            return false;
        }

        glViewport(0, 0, width, height);

        glfwSetWindowUserPointer(m_windowHandle, this);

        glfwSetFramebufferSizeCallback(m_windowHandle, framebuffer_resize_s);

        return true;
    }

    void GLDisplay::Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_windowHandle);
    }

    void GLDisplay::CleanUp()
    {
        glfwTerminate();

        if (m_windowHandle != NULL)
        {
            glfwDestroyWindow(m_windowHandle);
            m_windowHandle = NULL;
        }
    }

    void GLDisplay::framebuffer_resize(unsigned int width, unsigned int height)
    {
        glViewport(0, 0, width, height);
    }

}

void framebuffer_resize_s(GLFWwindow* window, int width, int height)
{
    GLFWwindow* currentWindow = glfwGetCurrentContext();
    PlanetoidEngine::GLDisplay* display = (PlanetoidEngine::GLDisplay*)glfwGetWindowUserPointer(currentWindow);
    if (display != NULL)
        display->framebuffer_resize(width, height);
}