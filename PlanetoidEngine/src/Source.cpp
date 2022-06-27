#include <iostream>

#include "GLDisplay.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
    PlanetoidEngine::GLDisplay display;

    if (display.Init(1280, 720, "Planetoid Test App") == false)
        return -1;

    while (true)
    {
        display.Update();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ESCAPE))
            break;
    }

    display.CleanUp();

    return 0;
}

