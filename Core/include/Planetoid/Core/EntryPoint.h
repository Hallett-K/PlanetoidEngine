#pragma once

#include <Planetoid/Core/Application.h>

extern PlanetoidEngine::Application* PlanetoidEngine::CreateApplication(PlanetoidEngine::CommandLineArgs commandLineArgs);

int main(int argc, char** argv)
{
    PlanetoidEngine::Application* app = PlanetoidEngine::CreateApplication({argc, argv});
    PlanetoidEngine::Application::s_Instance = app;

    app->commandLineArgs.argc = argc;
    app->commandLineArgs.argv = argv;

    app->Run();

    delete app;

    return 0;
}