#pragma once

#include <PlanetoidEngine.h>

#include <deque>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

#include "ProjectContext.h"

struct EditorContext
{
    PlanetoidEngine::File recentProjectList;
    std::vector<std::string> recentProjects;

    bool IsProjectLoaded = false;
    ProjectContext projectContext;

    PlanetoidEngine::Framebuffer framebuffer;
    PlanetoidEngine::Entity editorCameraEntity;
    PlanetoidEngine::Transform editorCameraTransform;
    PlanetoidEngine::Camera editorCameraComponent;

    std::deque<std::pair<unsigned int, std::string>> consoleMessages;
    bool consoleScrollToBottom = false;

    PlanetoidEngine::Entity selectedEntity;
    std::vector<std::string> selectedEntityComponents;
    int selectedEntityComponentIndex = -1;

    const ImGuiTreeNodeFlags assetBrowserTreeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    std::string assetBrowserSelectedPath = "";
    PlanetoidEngine::Texture folderTexture;
    PlanetoidEngine::Texture fileTexture;

    PlanetoidEngine::Texture playButtonTexture;
    PlanetoidEngine::Texture pauseButtonTexture;
    PlanetoidEngine::Texture stopButtonTexture;

    glm::vec2 viewportSize = glm::vec2(0.0f);
    ImGuiID dockspaceID = 0;
    
    bool editorCameraControls = false;

    bool showProjectWindow = false;
    bool closeProjectWindow = false;

    bool showAboutWindow = false;
    bool showNewProjectPopup = false;
    bool showOpenProjectPopup = false;

    bool showRenameEntityPopup = false;
    bool showNewScenePopup = false;
    bool showOpenScenePopup = false;

    bool showSceneSettingsPopup = false;

    bool showNewFolderPopup = false;
    bool showNewScriptPopup = false;

    enum class PlayState
    {
        Stopped,
        Playing,
        Paused
    };

    PlayState playState = PlayState::Stopped;
    bool sceneChange = false;

    PlanetoidEngine::Transform playInEditorCameraTransform;
    PlanetoidEngine::Camera playInEditorCameraComponent;
    std::unordered_map<std::string, std::string> playInEditorSceneCopies;
    bool playInEditorIsInterrupting = false;

    const unsigned int numRecentProjects = 3;

    const unsigned int majorVersion = 0;
    const unsigned int minorVersion = 1;
    const unsigned int patchVersion = 0;

    std::string GetVersionString() const
    {
        return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(patchVersion);
    }
};