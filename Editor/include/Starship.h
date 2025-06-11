#pragma once

#include <PlanetoidEngine.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <windows.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "EditorContext.h"


class Starship : public PlanetoidEngine::Application
{
public:

    virtual void Init() override;
    virtual void Update(float deltaTime) override;
    virtual void Shutdown() override;

protected:
    Starship(PlanetoidEngine::CommandLineArgs args)
        : Application(args)
    {
        windowProperties.mode = PlanetoidEngine::ScreenMode::Windowed;
        std::string title = "Starship Editor " + m_editorContext.GetVersionString();
        windowProperties.title = title;
        windowProperties.width = 1280;
        windowProperties.height = 720;
        windowProperties.startMaximised = true;
    }

    ~Starship()
    {

    }

    PlanetoidEngine::FileChangedDelegate::DelegateIterator m_fileChangedDelegateIterator;

    std::thread fileWatcherThread;
    std::atomic<bool> m_EnableFileWatcher = true;
    std::mutex m_vfsMutex;

    void FileWatcherTask()
    {
        while (m_EnableFileWatcher)
        {
            vfs.UpdateWatchedFiles();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        PlanetoidEngine::Log::Info("File watcher thread stopped");
    }

private:
    friend PlanetoidEngine::Application* PlanetoidEngine::CreateApplication(CommandLineArgs commandLineArgs);

    EditorContext m_editorContext;

    void WriteRecentProjects()
    {
        if (m_editorContext.recentProjects.size() > m_editorContext.numRecentProjects)
        {
            m_editorContext.recentProjects.erase(m_editorContext.recentProjects.begin() + m_editorContext.numRecentProjects, m_editorContext.recentProjects.end());
        }
        m_editorContext.recentProjectList.WriteTextContents(m_editorContext.recentProjects);
    }

    // Utilities
    bool CreateNewBlankProject(const std::string& name, const std::string& rootPath);
    bool CreateNewBlankScene(const std::string& name);
    bool LoadProject(const std::string& path);
    bool LoadScene(const std::string& name);
    void LoadSceneForPlayInEditor();
    void CreateSceneForPlayInEditor();
    void OnLogMessage(PlanetoidEngine::LogLevel level, std::string message);

    void OnSceneLoaded();

    void Input();

    void SaveProject();
    void SaveScene();

    void SerializeEntity(PlanetoidEngine::Entity entity, PlanetoidEngine::File& file);

    void InitEditorCamera();

    bool TryLoadScriptDLL();
    void OnDLLChanged(const std::string& path);

    void OnPlay();
    void OnPause();
    void OnStop();

    void AttachEntityScripts(bool callOnAttach = true);
    void DetachEntityScripts(bool callOnDetach = true);

    // Project Window
    void ProjectWindow();
    void NewProjectPopup();
    void OpenProjectPopup();

    // Utility Windows
    void RenameEntityPopup();
    void NewScenePopup();
    void OpenScenePopup();
    void SceneSettingsPopup();

    void NewFolderPopup();
    void NewScriptPopup();

    // Main Editor Widgets
    void MainMenuBar();
    void AboutWindow();
    void MainDockSpace();
    void Viewport();
    void EntityList();
    void ComponentList();
    void Inspector();
    void AssetBrowser();
    void Console();
    void Toolbar();

    // Inspector Widgets
    void InspectorTransform();
    void InspectorSpriteRenderer();
    void InspectorCamera();
    void InspectorScript();
    void InspectorBoxCollider2D();
    void InspectorCircleCollider2D();
    void InspectorRigidBody2D();

    // Asset Browser Widgets
    void DrawFolderView();
    void DrawBrowserView();

    void DrawAssetBrowserTreeRoot(const std::string& path);
    void DrawAssetBrowserTreeNode(const std::string& path, bool recursive = false);
    void DrawAssetBrowserTreeNodesFromVector(const std::string& basePath, const std::vector<std::string>& paths);

    // Code Project Generation
    void GenerateCodeProject();
    void GenerateCodeProjectCMakeLists(PlanetoidEngine::File& file);
    void GenerateCodeProjectMain(PlanetoidEngine::File& file);
    void GenerateCodeProjectScriptCore(PlanetoidEngine::File& file);
    void GenerateCodeProjectRegisterScripts(PlanetoidEngine::File& file);
    void GenerateCodeProjectBuildBat(PlanetoidEngine::File& file);
};

PE_APP(Starship);