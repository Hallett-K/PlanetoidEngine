#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Core/AssetStore.h>
#include <Planetoid/Core/InputManager.h>
#include <Planetoid/Core/Log.h>
#include <Planetoid/Core/Window.h>
#include <Planetoid/Renderer/Renderer.h>
#include <Planetoid/Scenes/SceneManager.h>
#include <Planetoid/VirtualFileSystem/VirtualFileSystem.h>

#include <Planetoid/Renderer/Framebuffer.h>

int main(int argc, char** argv);
class ImGuiContext;

namespace PlanetoidEngine
{
    struct PE_API CommandLineArgs
    {
        int argc;
        char** argv;
    };

    class PE_API Application
    {
    public:
        static Application* GetInstance();

        virtual void Init();
        virtual void Update(float deltaTime);
        virtual void Shutdown();

        inline bool IsRunning() const { return m_Running; }
        void Exit();

    protected:
        WindowProperties windowProperties;
        Window window;
        SceneManager sceneManager;
        Renderer renderer;
        CommandLineArgs commandLineArgs;
        VirtualFileSystem vfs;
        AssetStore assetStore;

        ImGuiContext* imGuiContext;
        

        Application(CommandLineArgs commandLineArgs);
        virtual ~Application();

        
    private:
        friend int ::main(int argc, char** argv);

        bool m_Running = false;
        void Run();

        float m_targetFPS = 60.0f;
        
        friend class InputManager;
        friend class ScriptAPI;

        static Application* s_Instance;
        
    };  
    
    Application* CreateApplication(CommandLineArgs commandLineArgs);

    #define PE_APP(T) PlanetoidEngine::Application* PlanetoidEngine::CreateApplication(CommandLineArgs commandLineArgs) { return new T(commandLineArgs); }
}