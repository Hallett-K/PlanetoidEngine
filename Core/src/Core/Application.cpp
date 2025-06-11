#include <Planetoid/Core/Application.h>

#include <Planetoid/Core/Extensions.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Planetoid/Scenes/Components.h>
#include <Planetoid/Scenes/Scene.h>

#include <entt/entt.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace PlanetoidEngine
{
    Application* Application::s_Instance = nullptr;
    
    Application* Application::GetInstance()
    {
        return s_Instance;
    }

    Application::Application(CommandLineArgs args)
        : commandLineArgs(args)
    {
        
    }

    Application::~Application()
    {

    }

    void Application::Init()
    {
        
    }

    void Application::Update(float deltaTime)
    {

    }

    void Application::Shutdown()
    {

    }

    void Application::Exit()
    {
        m_Running = false;
    }

    void Application::Run()
    {
        m_Running = true;

        window.Init(windowProperties);
        InputManager::Init();
        renderer.Init();

        IMGUI_CHECKVERSION();
        imGuiContext = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        
        ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
        ImGui_ImplOpenGL3_Init("#version 460");

        ImGui::StyleColorsDark();

        // Setup Virtual File System initial mount points
        std::string engineRoot = commandLineArgs.argv[0];
        engineRoot = engineRoot.substr(0, engineRoot.find_last_of("/\\"));
        std::replace(engineRoot.begin(), engineRoot.end(), '\\', '/');

        vfs.Mount("EngineRoot", engineRoot);

        Init();

        Log::Info("Application initialized");

        // Get vendor and renderer
        std::string vendor = (char*)glGetString(GL_VENDOR);
        std::string renderer = (char*)glGetString(GL_RENDERER);
        std::string version = (char*)glGetString(GL_VERSION);

        Log::Info("OpenGL Vendor: " + vendor);
        Log::Info("OpenGL Renderer: " + renderer);
        Log::Info("OpenGL Version: " + version);

        if (Texture::IsBindlessTextureSupported())
        {
            Log::Success("Bindless textures supported");
        }
        else
        {
            Log::Error("Bindless textures not supported");

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            return;
        }

        srand(time(NULL));

        float timeElapsed = 0.0f;
        float lastFrameTime = glfwGetTime();
        float deltaTime = 0.0f;

        while (window.IsOpen() && m_Running)
        {
            float currentFrameTime = glfwGetTime();
            deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            window.SwapBuffers();
            window.PollEvents();

            InputManager::Poll(deltaTime);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();

            ImGui::NewFrame();

            Update(deltaTime);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
            { 
                GLFWwindow* backup_current_context = glfwGetCurrentContext(); 
                ImGui::UpdatePlatformWindows(); 
                ImGui::RenderPlatformWindowsDefault(); 
                glfwMakeContextCurrent(backup_current_context); 
            }
        }

        Shutdown();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}