#include <iostream>

#include <Planetoid/Core/UUID.h>

#include "Starship.h"
#include "Components.h"

#include <vector>

#include <imgui_stdlib.h>

#include <windows.h>
#include <stdio.h>

void Starship::Init() 
{
    ImGui::SetCurrentContext(imGuiContext);

    if (commandLineArgs.argc == 1)
    {
        m_editorContext.showProjectWindow = true;
        m_editorContext.closeProjectWindow = false;
    }

    m_editorContext.recentProjectList = PlanetoidEngine::FileIO::Open("recent_projects.txt");
    m_editorContext.recentProjects = m_editorContext.recentProjectList.ReadTextContentsToStringArray();
    if (m_editorContext.recentProjects.size() > m_editorContext.numRecentProjects)
    {
        m_editorContext.recentProjects.erase(m_editorContext.recentProjects.begin() + m_editorContext.numRecentProjects, m_editorContext.recentProjects.end());
    }

    bool listChanged = false;
    for (int i = 0; i < m_editorContext.recentProjects.size(); i++)
    {
        if (!PlanetoidEngine::FileIO::FileExists(m_editorContext.recentProjects[i]))
        {
            m_editorContext.recentProjects.erase(m_editorContext.recentProjects.begin() + i);
            listChanged = true;
        }
    }

    if (listChanged)
    {
        WriteRecentProjects();
    }

    m_editorContext.framebuffer.Init(1280, 720);

    renderer.setClearColour(0.2f, 0.3f, 0.8f);

    m_editorContext.folderTexture.LoadFromFile("assets/textures/opened-folder.png");
    m_editorContext.fileTexture.LoadFromFile("assets/textures/document.png");

    m_editorContext.playButtonTexture.LoadFromFile("assets/textures/play.png");
    m_editorContext.pauseButtonTexture.LoadFromFile("assets/textures/pause.png");
    m_editorContext.stopButtonTexture.LoadFromFile("assets/textures/stop.png");

    PlanetoidEngine::Log::OnLog.Bind(&Starship::OnLogMessage, this);

    m_EnableFileWatcher = true;
    fileWatcherThread = std::thread(&Starship::FileWatcherTask, this);

    window.SetWindowIcon("assets/textures/Starship.png");

    sceneManager.OnSceneEnter.Bind(&Starship::OnSceneLoaded, this);
}

void Starship::Update(float deltaTime)
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();

    // TODO: Enhanced Input System - we want to detect key presses and releases, not just if they are currently pressed
    //Input();

    if (scene)
    {
        switch (m_editorContext.playState)
        {
            case EditorContext::PlayState::Playing:
            {
                scene->GetRegistry().view<PlanetoidEngine::ScriptComponent>().each([&](auto& scriptComponent)
                {
                    if (scriptComponent.script)
                        scriptComponent.script->OnUpdate(deltaTime);
                });

                scene->OnUpdate(deltaTime);

                renderer.BeginScene(m_editorContext.playInEditorCameraTransform.GetMatrix(), m_editorContext.playInEditorCameraComponent.GetProjection());
                scene->GetRegistry().view<PlanetoidEngine::Transform, PlanetoidEngine::SpriteRenderer>().each([&](auto& transform, auto& spriteRenderer)
                {
                    renderer.Submit(transform, spriteRenderer.color, spriteRenderer.texture);
                });
                m_editorContext.framebuffer.Bind();
                renderer.Flush();
                m_editorContext.framebuffer.Unbind();    

                break;
            }

            case EditorContext::PlayState::Paused:
            {
                renderer.BeginScene(m_editorContext.playInEditorCameraTransform.GetMatrix(), m_editorContext.playInEditorCameraComponent.GetProjection());
                scene->GetRegistry().view<PlanetoidEngine::Transform, PlanetoidEngine::SpriteRenderer>().each([&](auto& transform, auto& spriteRenderer)
                {
                    renderer.Submit(transform, spriteRenderer.color, spriteRenderer.texture);
                });
                m_editorContext.framebuffer.Bind();
                renderer.Flush();
                m_editorContext.framebuffer.Unbind();    

                break;
            }

            case EditorContext::PlayState::Stopped:
            {
                if (m_editorContext.editorCameraControls)
                {
                    if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_W))
                    {
                        m_editorContext.editorCameraTransform.SetPosition(m_editorContext.editorCameraTransform.GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f) * deltaTime * 600.0f);
                    }
                    if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_S))
                    {
                        m_editorContext.editorCameraTransform.SetPosition(m_editorContext.editorCameraTransform.GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * 600.0f);
                    }
                    if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_A))
                    {
                        m_editorContext.editorCameraTransform.SetPosition(m_editorContext.editorCameraTransform.GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * 600.0f);
                    }
                    if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_D))
                    {
                        m_editorContext.editorCameraTransform.SetPosition(m_editorContext.editorCameraTransform.GetPosition() + glm::vec3(-1.0f, 0.0f, 0.0f) * deltaTime * 600.0f);
                    }

                    if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_F))
                    {
                        if (m_editorContext.selectedEntity != PlanetoidEngine::Entity())
                        {
                            if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Transform>())
                            {
                                PlanetoidEngine::Transform& transform = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::Transform>();
                                glm::vec3 position = transform.GetPosition();
                                position.x -= m_editorContext.editorCameraComponent.right / 2.0f;
                                position.y -= m_editorContext.editorCameraComponent.top / 2.0f;
                                m_editorContext.editorCameraTransform.SetPosition(-position);
                            }
                        }
                    }
                }

                renderer.BeginScene(m_editorContext.editorCameraTransform.GetMatrix(), m_editorContext.editorCameraComponent.GetProjection());
                scene->GetRegistry().view<PlanetoidEngine::Transform, PlanetoidEngine::SpriteRenderer>().each([&](auto& transform, auto& spriteRenderer)
                {
                    renderer.Submit(transform, spriteRenderer.color, spriteRenderer.texture);
                });
                m_editorContext.framebuffer.Bind();
                renderer.Flush();
                m_editorContext.framebuffer.Unbind();

                break;
            }
        }
    }
    else
    {
        renderer.BeginScene(m_editorContext.editorCameraTransform.GetMatrix(), m_editorContext.editorCameraComponent.GetProjection());
        m_editorContext.framebuffer.Bind();
        renderer.Flush();
        m_editorContext.framebuffer.Unbind();
    }

    ProjectWindow();
    NewProjectPopup();
    
    MainDockSpace();
    MainMenuBar();
    Viewport();
    AboutWindow();
    EntityList();
    ComponentList();
    AssetBrowser();
    Console();
    Toolbar();

    RenameEntityPopup();
    NewScenePopup();
    OpenScenePopup();
    NewFolderPopup();
    NewScriptPopup();
}

void Starship::Shutdown()
{
    PlanetoidEngine::InputManager::ClearInputs();
    PlanetoidEngine::InputManager::OnFileDrop.UnbindAll();
    PlanetoidEngine::InputManager::OnScrollWheel.UnbindAll();

    m_EnableFileWatcher.store(false);
    if (fileWatcherThread.joinable())
    {
        fileWatcherThread.join();
    }

    vfs.OnWatchedFileChanged.Unbind(m_fileChangedDelegateIterator);

    if (m_editorContext.projectContext.dll)
    {
        m_editorContext.projectContext.OnDetachFunc();
        FreeLibrary(m_editorContext.projectContext.dll);
        m_editorContext.projectContext.dll = nullptr;
    }

    std::string dllCopyPath = "CodeProjectRoot/bin/libPlanetoidEngine-" + m_editorContext.projectContext.name + "-ScriptEngine-Copy.dll";
    std::string dllCopyPhysicalPath = vfs.ResolveVirtualPath(dllCopyPath);

    if (PlanetoidEngine::FileIO::FileExists(dllCopyPhysicalPath))
    {
        PlanetoidEngine::FileIO::FileDelete(dllCopyPhysicalPath);
    }
}

// Utilities

bool Starship::CreateNewBlankProject(const std::string& name, const std::string& rootPath)
{
    std::string projectPathStr = rootPath;
            
    projectPathStr += "/" + std::string(name);
    if (!PlanetoidEngine::FileIO::FolderExists(projectPathStr))
    {
        PlanetoidEngine::FileIO::FolderCreate(projectPathStr);
        PlanetoidEngine::FileIO::FolderCreate(projectPathStr + "/GlobalAssets");
        PlanetoidEngine::FileIO::FolderCreate(projectPathStr + "/Scenes");
        PlanetoidEngine::File projectShipFile = PlanetoidEngine::FileIO::Open(projectPathStr + "/" + std::string(name) + ".ship");
        projectShipFile.WriteTextContents("Name:" + std::string(name) + ",EditorVersion:" + m_editorContext.GetVersionString() + ",StartupScene:main");

        m_editorContext.projectContext.name = name;
        m_editorContext.projectContext.rootPath = projectPathStr;
        m_editorContext.projectContext.startupSceneName = "main";
    
        CreateNewBlankScene("main");
        sceneManager.SetCurrentScene("main");
        m_editorContext.projectContext.currentSceneName = "main";

        vfs.Mount("ProjectRoot", projectPathStr);
        vfs.Mount("GlobalAssets", projectPathStr + "/GlobalAssets");
        vfs.Mount("Scenes", projectPathStr + "/Scenes");

        InitEditorCamera();

        GenerateCodeProject();

        PlanetoidEngine::Log::Success("Created new project: " + name);

        return true;
    }

    return false;
}

bool Starship::CreateNewBlankScene(const std::string& name)
{
    if (m_editorContext.projectContext.rootPath == "")
    {
        return false;
    }
    if (sceneManager.IsSceneCreated(name))
    {
        return false;
    }

    if (PlanetoidEngine::FileIO::FolderExists(m_editorContext.projectContext.rootPath + "/Scenes/" + name))
    {
        return false;
    }

    PlanetoidEngine::FileIO::FolderCreate(m_editorContext.projectContext.rootPath + "/Scenes/" + name);
    PlanetoidEngine::File sceneFile = PlanetoidEngine::FileIO::Open(m_editorContext.projectContext.rootPath + "/Scenes/" + name + "/" + name + ".scene");
    sceneFile.WriteTextContents("Name:" + name + ",Entities:");

    PlanetoidEngine::FileIO::FolderCreate(m_editorContext.projectContext.rootPath + "/Scenes/" + name + "/SceneAssets");

    sceneManager.CreateScene(name);

    vfs.Mount("SceneRoot", m_editorContext.projectContext.rootPath + "/Scenes/" + name);
    vfs.Mount("SceneAssets", m_editorContext.projectContext.rootPath + "/Scenes/" + name + "/SceneAssets");

    PlanetoidEngine::Log::Info("Created new scene: " + name);

    return true;
}

bool Starship::LoadProject(const std::string& path)
{
    if (PlanetoidEngine::FileIO::FileExists(path))
    {
        // Get the project name from the path
        std::string projectName = path.substr(path.find_last_of("/") + 1);
        // Trim the extension
        projectName = projectName.substr(0, projectName.find_last_of("."));

        m_editorContext.projectContext.name = projectName;
        m_editorContext.projectContext.rootPath = path.substr(0, path.find_last_of("/"));

        PlanetoidEngine::File projectFile = PlanetoidEngine::FileIO::Open(path);
        std::string projectFileContents = projectFile.ReadTextContentsToString();

        size_t namePos = projectFileContents.find("Name:");
        size_t editorVersionPos = projectFileContents.find("EditorVersion:");
        size_t startupScenePos = projectFileContents.find("StartupScene:");

        if (namePos == std::string::npos || editorVersionPos == std::string::npos || startupScenePos == std::string::npos)
        {
            return false;
        }

        std::string name = projectFileContents.substr(namePos + 5, editorVersionPos - namePos - 6);
        std::string editorVersion = projectFileContents.substr(editorVersionPos + 14, startupScenePos - editorVersionPos - 15);
        std::string startupScene = projectFileContents.substr(startupScenePos + 13);

        if (editorVersion != m_editorContext.GetVersionString())
        {
            return false;
        }

        vfs.Mount("ProjectRoot", m_editorContext.projectContext.rootPath);
        vfs.Mount("GlobalAssets", m_editorContext.projectContext.rootPath + "/GlobalAssets");
        vfs.Mount("Scenes", m_editorContext.projectContext.rootPath + "/Scenes");

        if (startupScene != "")
        {
            vfs.Mount("SceneRoot", m_editorContext.projectContext.rootPath + "/Scenes/" + startupScene);
            vfs.Mount("SceneAssets", m_editorContext.projectContext.rootPath + "/Scenes/" + startupScene + "/SceneAssets");
            if (LoadScene(startupScene))
            {
                //sceneManager.SetCurrentScene(startupScene);
                InitEditorCamera();
            }
        }

        std::string CodeProjectRoot = vfs.ResolveVirtualPath("ProjectRoot/ScriptEngine");
        bool tryLoad = false;
        if (!PlanetoidEngine::FileIO::FolderExists(CodeProjectRoot))
        {
            PlanetoidEngine::Log::Warn("Code Project does not exist - generating");
            GenerateCodeProject();
        }
        else
        {
            tryLoad = true;
            vfs.Mount("CodeProjectRoot", CodeProjectRoot);
        }

        if (tryLoad)
        {
            TryLoadScriptDLL();
            AttachEntityScripts(false);
        }
        
        PlanetoidEngine::Log::Success("Loaded Project " + projectName);

        return true;
    }

    return false;
}

bool Starship::LoadScene(const std::string& name)
{
    if (m_editorContext.projectContext.rootPath == "")
    {
        return false;
    }
    if (sceneManager.IsSceneCreated(name))
    {
        sceneManager.DestroyScene(name);
    }
    if (!PlanetoidEngine::FileIO::FolderExists(m_editorContext.projectContext.rootPath + "/Scenes/" + name))
    {
        return false;
    }
    if (!PlanetoidEngine::FileIO::FileExists(m_editorContext.projectContext.rootPath + "/Scenes/" + name + "/" + name + ".scene"))
    {
        return false;
    }

    sceneManager.CreateScene(name, true);
    m_editorContext.projectContext.currentSceneName = name;

    vfs.Mount("SceneRoot", m_editorContext.projectContext.rootPath + "/Scenes/" + name);
    vfs.Mount("SceneAssets", m_editorContext.projectContext.rootPath + "/Scenes/" + name + "/SceneAssets");

    // TODO: Entity loading
    PlanetoidEngine::File sceneFile = PlanetoidEngine::FileIO::Open(m_editorContext.projectContext.rootPath + "/Scenes/" + name + "/" + name + ".scene");
    std::vector<std::string> sceneFileContents = sceneFile.ReadTextContentsToStringArray();
    
    // Skip first line, we already know scene name
    PlanetoidEngine::Entity entity;
    for (int i = 1; i < sceneFileContents.size(); i++)
    {
        std::string line = sceneFileContents[i];
        if (line.find("Entity:") != std::string::npos)
        {
            // Entity:EntityName-UUID1-UUID2
            std::string entityName = line.substr(7, line.find("-") - 7);
            std::string uuid = line.substr(line.find("-") + 1);

            entity = sceneManager.GetCurrentScene()->CreateEntity(PlanetoidEngine::UUID(uuid), entityName);

            continue;
        }
        else if (line.find("Transform:") != std::string::npos)
        {
            // Transform:posx,posy,rotation,scalex,scaley
            std::string transformData = line.substr(10);
            std::string posX = transformData.substr(0, transformData.find(","));
            transformData = transformData.substr(transformData.find(",") + 1);
            std::string posY = transformData.substr(0, transformData.find(","));
            transformData = transformData.substr(transformData.find(",") + 1);
            std::string posZ = transformData.substr(0, transformData.find(","));
            transformData = transformData.substr(transformData.find(",") + 1);
            std::string rotation = transformData.substr(0, transformData.find(","));
            transformData = transformData.substr(transformData.find(",") + 1);
            std::string scaleX = transformData.substr(0, transformData.find(","));
            transformData = transformData.substr(transformData.find(",") + 1);
            std::string scaleY = transformData;

            PlanetoidEngine::Transform& transform = entity.AddComponent<PlanetoidEngine::Transform>();
            transform.SetPosition(glm::vec3(std::stof(posX), std::stof(posY), std::stof(posZ)));
            transform.SetRotation(std::stof(rotation));
            transform.SetScale(glm::vec2(std::stof(scaleX), std::stof(scaleY)));

            continue;
        }
        else if (line.find("SpriteRenderer:") != std::string::npos)
        {
            // SpriteRenderer:r,g,b,a
            std::string spriteRendererData = line.substr(15);
            std::string r = spriteRendererData.substr(0, spriteRendererData.find(","));
            spriteRendererData = spriteRendererData.substr(spriteRendererData.find(",") + 1);
            std::string g = spriteRendererData.substr(0, spriteRendererData.find(","));
            spriteRendererData = spriteRendererData.substr(spriteRendererData.find(",") + 1);
            std::string b = spriteRendererData.substr(0, spriteRendererData.find(","));
            spriteRendererData = spriteRendererData.substr(spriteRendererData.find(",") + 1);
            std::string a = spriteRendererData;

            PlanetoidEngine::SpriteRenderer& spriteRenderer = entity.AddComponent<PlanetoidEngine::SpriteRenderer>();
            spriteRenderer.color = glm::vec4(std::stof(r), std::stof(g), std::stof(b), std::stof(a));

            std::string textureData = sceneFileContents[++i];
            // \tTexture:width,height,channels,sourceType,sourcePath
            std::string width = textureData.substr(9, textureData.find(",") - 9);
            textureData = textureData.substr(textureData.find(",") + 1);
            std::string height = textureData.substr(0, textureData.find(","));
            textureData = textureData.substr(textureData.find(",") + 1);
            std::string channels = textureData.substr(0, textureData.find(","));
            textureData = textureData.substr(textureData.find(",") + 1);
            std::string sourceType = textureData.substr(0, textureData.find(","));
            int sourceTypeInt = std::stoi(sourceType);
            if (sourceTypeInt == 0)
            {
                // Uninitialized
                continue;
            }
            else if (sourceTypeInt == 1)
            {
                // File
                std::string sourcePath = textureData.substr(textureData.find(",") + 1);
                PlanetoidEngine::Texture* texture = new PlanetoidEngine::Texture();
                texture->LoadFromFile(vfs.ResolveVirtualPath(sourcePath));
                spriteRenderer.texture = texture;

                continue;
            }
            else if (sourceTypeInt == 2)
            {
                // Blank
                PlanetoidEngine::Texture* texture = new PlanetoidEngine::Texture();
                texture->White(std::stoi(width), std::stoi(height), std::stoi(channels));
                spriteRenderer.texture = texture;

                continue;
            }
            else if (sourceTypeInt == 3)
            {
                // RandomNoise
                PlanetoidEngine::Texture* texture = new PlanetoidEngine::Texture();
                texture->RandomNoise(std::stoi(width), std::stoi(height), std::stoi(channels));
                spriteRenderer.texture = texture;

                continue;
            }
        }
        else if (line.find("Camera:") != std::string::npos)
        {
            // Camera:left,right,bottom,top,near,far
            std::string cameraData = line.substr(7);
            std::string left = cameraData.substr(0, cameraData.find(","));
            cameraData = cameraData.substr(cameraData.find(",") + 1);
            std::string right = cameraData.substr(0, cameraData.find(","));
            cameraData = cameraData.substr(cameraData.find(",") + 1);
            std::string bottom = cameraData.substr(0, cameraData.find(","));
            cameraData = cameraData.substr(cameraData.find(",") + 1);
            std::string top = cameraData.substr(0, cameraData.find(","));
            cameraData = cameraData.substr(cameraData.find(",") + 1);
            std::string nearPlane = cameraData.substr(0, cameraData.find(","));
            cameraData = cameraData.substr(cameraData.find(",") + 1);
            std::string farPlane = cameraData;

            PlanetoidEngine::Camera& camera = entity.AddComponent<PlanetoidEngine::Camera>();
            camera.SetProjection(std::stof(left), std::stof(right), std::stof(bottom), std::stof(top), std::stof(nearPlane), std::stof(farPlane));

            continue;
        }
        else if (line.find("ScriptComponent:") != std::string::npos)
        {
            // ScriptComponent:scriptPath
            std::string scriptPath = line.substr(16);
            
            PlanetoidEngine::ScriptComponent& scriptComponent = entity.AddComponent<PlanetoidEngine::ScriptComponent>();
            scriptComponent.scriptPath = scriptPath;

            continue;
        }
        else if (line.find("BoxCollider2D:") != std::string::npos)
        {
            // BoxCollider2D:sizeX,sizeY,offsetX,offsetY,density,friction,restitution,restitutionThreshold,scaleWithTransform
            std::string boxColliderData = line.substr(14);
            std::string sizeX = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string sizeY = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string offsetX = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string offsetY = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string density = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string friction = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string restitution = boxColliderData.substr(0, boxColliderData.find(","));
            boxColliderData = boxColliderData.substr(boxColliderData.find(",") + 1);
            std::string scaleWithTransform = boxColliderData;

            PlanetoidEngine::BoxCollider2D& boxCollider = entity.AddComponent<PlanetoidEngine::BoxCollider2D>();
            boxCollider.size = glm::vec2(std::stof(sizeX), std::stof(sizeY));
            boxCollider.offset = glm::vec2(std::stof(offsetX), std::stof(offsetY));
            boxCollider.density = std::stof(density);
            boxCollider.friction = std::stof(friction);
            boxCollider.restitution = std::stof(restitution);
            boxCollider.scaleWithTransform = std::stoi(scaleWithTransform);

            continue;
        }
        else if (line.find("CircleCollider2D:") != std::string::npos)
        {
            // CircleCollider2D:radius,offsetX,offsetY,density,friction,restitution,restitutionThreshold,scaleWithTransform
            std::string circleColliderData = line.substr(17);
            std::string radius = circleColliderData.substr(0, circleColliderData.find(","));
            circleColliderData = circleColliderData.substr(circleColliderData.find(",") + 1);
            std::string offsetX = circleColliderData.substr(0, circleColliderData.find(","));
            circleColliderData = circleColliderData.substr(circleColliderData.find(",") + 1);
            std::string offsetY = circleColliderData.substr(0, circleColliderData.find(","));
            circleColliderData = circleColliderData.substr(circleColliderData.find(",") + 1);
            std::string density = circleColliderData.substr(0, circleColliderData.find(","));
            circleColliderData = circleColliderData.substr(circleColliderData.find(",") + 1);
            std::string friction = circleColliderData.substr(0, circleColliderData.find(","));
            circleColliderData = circleColliderData.substr(circleColliderData.find(",") + 1);
            std::string restitution = circleColliderData.substr(0, circleColliderData.find(","));
            circleColliderData = circleColliderData.substr(circleColliderData.find(",") + 1);
            std::string scaleWithTransform = circleColliderData;

            PlanetoidEngine::CircleCollider2D& circleCollider = entity.AddComponent<PlanetoidEngine::CircleCollider2D>();
            circleCollider.radius = std::stof(radius);
            circleCollider.offset = glm::vec2(std::stof(offsetX), std::stof(offsetY));
            circleCollider.density = std::stof(density);
            circleCollider.friction = std::stof(friction);
            circleCollider.restitution = std::stof(restitution);
            circleCollider.scaleWithTransform = std::stoi(scaleWithTransform);

            continue;
        }
        else if (line.find("RigidBody2D:") != std::string::npos)
        {
            // RigidBody2D:bodyType,allowRotation
            std::string rigidBody2DData = line.substr(12);
            std::string bodyType = rigidBody2DData.substr(0, rigidBody2DData.find(","));
            rigidBody2DData = rigidBody2DData.substr(rigidBody2DData.find(",") + 1);
            std::string allowRotation = rigidBody2DData;

            PlanetoidEngine::RigidBody2D& rigidBody2D = entity.AddComponent<PlanetoidEngine::RigidBody2D>();
            rigidBody2D.type = (PlanetoidEngine::RigidBody2D::RigidBodyType)std::stoi(bodyType);
            rigidBody2D.allowRotation = std::stoi(allowRotation);

            continue;
        }
    }

    m_editorContext.selectedEntity = PlanetoidEngine::Entity();

    PlanetoidEngine::Log::Success("Successfully loaded Scene " + name);

    return true;
}

void Starship::LoadSceneForPlayInEditor()
{
    std::string sceneName = sceneManager.GetCurrentSceneName();
    if (m_editorContext.playInEditorSceneCopies.find(sceneName) != m_editorContext.playInEditorSceneCopies.end())
    {
        sceneManager.SetCurrentScene(m_editorContext.playInEditorSceneCopies[sceneName]);
        sceneManager.GetCurrentScene()->OnLoad();
    }
    else
    {
        CreateSceneForPlayInEditor();
    }
}

void Starship::CreateSceneForPlayInEditor()
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    std::string editorSceneName = sceneManager.GetCurrentSceneName();
    // Create a copy of the scene
    PlanetoidEngine::UUID sceneUUID;
    std::string sceneName = editorSceneName + "-PlayScene-" + sceneUUID.ToString();
    sceneManager.CreateScene(sceneName, true);

    m_editorContext.playInEditorSceneCopies[editorSceneName] = sceneName;

    PlanetoidEngine::Scene* playScene = sceneManager.GetScene(sceneName);

    // Copy all entities from the current scene to the play scene
    std::vector<PlanetoidEngine::Entity> entities = scene->GetAllEntities();
    for (int i = 0; i < entities.size(); i++)
    {
        PlanetoidEngine::Entity entity = playScene->CreateEntity(entities[i].GetComponent<PlanetoidEngine::UUID>(), entities[i].GetComponent<PlanetoidEngine::NameComponent>().name);
        if (entities[i].HasComponent<PlanetoidEngine::Transform>())
        {
            PlanetoidEngine::Transform& transform = entities[i].GetComponent<PlanetoidEngine::Transform>();
            PlanetoidEngine::Transform& newTransform = entity.AddComponent<PlanetoidEngine::Transform>();
            newTransform.SetPosition(transform.GetPosition());
            newTransform.SetRotation(transform.GetRotation());
            newTransform.SetScale(transform.GetScale());
        }
        if (entities[i].HasComponent<PlanetoidEngine::SpriteRenderer>())
        {
            PlanetoidEngine::SpriteRenderer& spriteRenderer = entities[i].GetComponent<PlanetoidEngine::SpriteRenderer>();
            PlanetoidEngine::SpriteRenderer& newSpriteRenderer = entity.AddComponent<PlanetoidEngine::SpriteRenderer>();
            newSpriteRenderer.color = spriteRenderer.color;
            newSpriteRenderer.texture = spriteRenderer.texture;
        }
        if (entities[i].HasComponent<PlanetoidEngine::Camera>())
        {
            PlanetoidEngine::Camera& camera = entities[i].GetComponent<PlanetoidEngine::Camera>();
            PlanetoidEngine::Camera& newCamera = entity.AddComponent<PlanetoidEngine::Camera>();
            newCamera.SetProjection(camera.left, camera.right, camera.bottom, camera.top, camera.nearPlane, camera.farPlane);
        }
        if (entities[i].HasComponent<PlanetoidEngine::ScriptComponent>())
        {
            PlanetoidEngine::ScriptComponent& scriptComponent = entities[i].GetComponent<PlanetoidEngine::ScriptComponent>();
            PlanetoidEngine::ScriptComponent& newScriptComponent = entity.AddComponent<PlanetoidEngine::ScriptComponent>();
            newScriptComponent.scriptPath = scriptComponent.scriptPath;
        }
        if (entities[i].HasComponent<PlanetoidEngine::RigidBody2D>())
        {
            PlanetoidEngine::RigidBody2D& rigidBody2D = entities[i].GetComponent<PlanetoidEngine::RigidBody2D>();
            PlanetoidEngine::RigidBody2D& newRigidBody2D = entity.AddComponent<PlanetoidEngine::RigidBody2D>();
            newRigidBody2D.type = rigidBody2D.type;
            newRigidBody2D.allowRotation = rigidBody2D.allowRotation;
        }
        if (entities[i].HasComponent<PlanetoidEngine::BoxCollider2D>())
        {
            PlanetoidEngine::BoxCollider2D& boxCollider2D = entities[i].GetComponent<PlanetoidEngine::BoxCollider2D>();
            PlanetoidEngine::BoxCollider2D& newBoxCollider2D = entity.AddComponent<PlanetoidEngine::BoxCollider2D>();
            newBoxCollider2D.size = boxCollider2D.size;
            newBoxCollider2D.offset = boxCollider2D.offset;
            newBoxCollider2D.density = boxCollider2D.density;
            newBoxCollider2D.friction = boxCollider2D.friction;
            newBoxCollider2D.restitution = boxCollider2D.restitution;
            newBoxCollider2D.scaleWithTransform = boxCollider2D.scaleWithTransform;
        }
        if (entities[i].HasComponent<PlanetoidEngine::CircleCollider2D>())
        {
            PlanetoidEngine::CircleCollider2D& circleCollider2D = entities[i].GetComponent<PlanetoidEngine::CircleCollider2D>();
            PlanetoidEngine::CircleCollider2D& newCircleCollider2D = entity.AddComponent<PlanetoidEngine::CircleCollider2D>();
            newCircleCollider2D.radius = circleCollider2D.radius;
            newCircleCollider2D.offset = circleCollider2D.offset;
            newCircleCollider2D.density = circleCollider2D.density;
            newCircleCollider2D.friction = circleCollider2D.friction;
            newCircleCollider2D.restitution = circleCollider2D.restitution;
            newCircleCollider2D.scaleWithTransform = circleCollider2D.scaleWithTransform;
        }
    }
}

void Starship::OnLogMessage(PlanetoidEngine::LogLevel level, std::string message)
{
    m_editorContext.consoleMessages.push_back(std::make_pair<unsigned int, std::string>((unsigned int)level, std::move(message)));
    m_editorContext.consoleScrollToBottom = true;
}

void Starship::OnSceneLoaded()
{
    if (m_editorContext.playState == EditorContext::PlayState::Playing && !m_editorContext.playInEditorIsInterrupting)
    {
        sceneManager.InterruptLoad();
        m_editorContext.playInEditorIsInterrupting = true;
        LoadSceneForPlayInEditor();
        m_editorContext.playInEditorIsInterrupting = false;
    }
}

void Starship::Input()
{
    if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_LEFT_CONTROL))
    {   
        // Shortcuts

        // CTRL-S - save scene
        if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_S))
        {
            if (PlanetoidEngine::InputManager::IsKeyPressed(PE_KEY_LEFT_SHIFT))
            {
                SaveProject();
            }
            else
            {
                SaveScene();
            }
        }
        // CTRL-SHIFT-S - save project
        // CTRL-N - new scene
    }
}

void Starship::SaveProject()
{
    if (m_editorContext.projectContext.rootPath == "")
    {
        return;
    }
    if (m_editorContext.projectContext.name == "")
    {
        return;
    }
    if (m_editorContext.playState == EditorContext::PlayState::Playing)
    {
        PlanetoidEngine::Log::Error("Cannot save project while playing");
        return;
    }
    if (m_editorContext.projectContext.startupSceneName == "")
    {
        m_editorContext.projectContext.startupSceneName = "main";
    }

    PlanetoidEngine::File projectFile = PlanetoidEngine::FileIO::Open(m_editorContext.projectContext.rootPath + "/" + m_editorContext.projectContext.name + ".ship");
    projectFile.WriteTextContents("Name:" + m_editorContext.projectContext.name + ",EditorVersion:" + m_editorContext.GetVersionString() + ",StartupScene:" + m_editorContext.projectContext.startupSceneName);

    PlanetoidEngine::Log::Info("Saved Project " + m_editorContext.projectContext.name);
}

void Starship::SaveScene()
{
    if (m_editorContext.projectContext.rootPath == "")
    {
        return;
    }
    if (m_editorContext.projectContext.currentSceneName == "")
    {
        return;
    }
    if (!sceneManager.GetCurrentScene())
    {
        return;
    }

    if (!PlanetoidEngine::FileIO::FolderExists(m_editorContext.projectContext.rootPath + "/Scenes/" + m_editorContext.projectContext.currentSceneName))
    {
        CreateNewBlankScene(m_editorContext.projectContext.currentSceneName);
    }

    PlanetoidEngine::File sceneFile = PlanetoidEngine::FileIO::Open(m_editorContext.projectContext.rootPath + "/Scenes/" + m_editorContext.projectContext.currentSceneName + "/" + m_editorContext.projectContext.currentSceneName + ".scene");
    sceneFile.WriteTextContents("Name:" + m_editorContext.projectContext.currentSceneName + ",Entities:\n");

    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    std::vector<PlanetoidEngine::Entity> entities = scene->GetAllEntities();
    for (int i = 0; i < entities.size(); i++)
    {
        SerializeEntity(entities[i], sceneFile);
    }

    PlanetoidEngine::Log::Info("Saved Scene " + m_editorContext.projectContext.currentSceneName);
}

void Starship::SerializeEntity(PlanetoidEngine::Entity entity, PlanetoidEngine::File& sceneFile)
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    PlanetoidEngine::Entity* entityPtr = &entity;

    std::string name = entity.GetComponent<PlanetoidEngine::NameComponent>().name;
    PlanetoidEngine::UUID uuid = entity.GetComponent<PlanetoidEngine::UUID>();

    if (entity.HasComponent<EditorExcludeComponent>())
    {
        return;
    }

    sceneFile.AppendTextContents("Entity:" + name + "-" + uuid.ToString() + "\n");
    if (entity.HasComponent<PlanetoidEngine::Transform>())
    {
        PlanetoidEngine::Transform& transform = entity.GetComponent<PlanetoidEngine::Transform>();
        glm::vec3 position = transform.GetPosition();
        glm::vec2 scale = transform.GetScale();
        float rotation = transform.GetRotation();

        sceneFile.AppendTextContents("Transform:" + std::to_string(position.x) + "," + std::to_string(position.y) + "," + std::to_string(position.z) + "," + std::to_string(rotation) + "," + std::to_string(scale.x) + "," + std::to_string(scale.y) + "\n");
    }
    if (entity.HasComponent<PlanetoidEngine::SpriteRenderer>())
    {
        PlanetoidEngine::SpriteRenderer& spriteRenderer = entity.GetComponent<PlanetoidEngine::SpriteRenderer>();
        glm::vec4 color = spriteRenderer.color;
        PlanetoidEngine::Texture* texture = spriteRenderer.texture;

        sceneFile.AppendTextContents("SpriteRenderer:" + std::to_string(color.r) + "," + std::to_string(color.g) + "," + std::to_string(color.b) + "," + std::to_string(color.a) + "\n");
        sceneFile.AppendTextContents("\tTexture:" + std::to_string(texture->Width) + "," + std::to_string(texture->Height) + "," + std::to_string(texture->Channels) + "," + std::to_string((int)texture->GetSourceType()));
        if (texture->GetSourceType() == PlanetoidEngine::Texture::SourceType::File)
            sceneFile.AppendTextContents("," + vfs.ResolvePath(texture->GetPath()) + "\n");
        else
            sceneFile.AppendTextContents("\n");
    }
    if (entity.HasComponent<PlanetoidEngine::Camera>())
    {
        PlanetoidEngine::Camera& camera = entity.GetComponent<PlanetoidEngine::Camera>();

        // Left,Right,Bottom,Top,Near,Far
        float left = camera.left;
        float right = camera.right;
        float bottom = camera.bottom;
        float top = camera.top;
        float nearPlane = camera.nearPlane;
        float farPlane = camera.farPlane;

        sceneFile.AppendTextContents("Camera:" + std::to_string(left) + "," + std::to_string(right) + "," + std::to_string(bottom) + "," + std::to_string(top) + "," + std::to_string(nearPlane) + "," + std::to_string(farPlane) + "\n");
    }
    if (entity.HasComponent<PlanetoidEngine::ScriptComponent>())
    {
        PlanetoidEngine::ScriptComponent& scriptComponent = entity.GetComponent<PlanetoidEngine::ScriptComponent>();

        sceneFile.AppendTextContents("ScriptComponent:" + scriptComponent.scriptPath + "\n");
    }
    if (entity.HasComponent<PlanetoidEngine::RigidBody2D>())
    {
        PlanetoidEngine::RigidBody2D& rigidBody2D = entity.GetComponent<PlanetoidEngine::RigidBody2D>();

        int bodyType = (int)rigidBody2D.type;
        int allowRotation = (int)rigidBody2D.allowRotation;

        sceneFile.AppendTextContents("RigidBody2D:" + std::to_string(bodyType) + "," + std::to_string(allowRotation) + "\n");
    }
    if (entity.HasComponent<PlanetoidEngine::BoxCollider2D>())
    {
        PlanetoidEngine::BoxCollider2D& boxCollider2D = entity.GetComponent<PlanetoidEngine::BoxCollider2D>();

        glm::vec2 size = boxCollider2D.size;
        glm::vec2 offset = boxCollider2D.offset;

        float density = boxCollider2D.density;
        float friction = boxCollider2D.friction;
        float restitution = boxCollider2D.restitution;
        int scaleWithTransform = (int)boxCollider2D.scaleWithTransform;

        sceneFile.AppendTextContents("BoxCollider2D:" + std::to_string(size.x) + "," + std::to_string(size.y) + "," + std::to_string(offset.x) + "," + std::to_string(offset.y) + "," + std::to_string(density) + "," + std::to_string(friction) + "," + std::to_string(restitution) + "," + std::to_string(scaleWithTransform) + "\n");
    }
    if (entity.HasComponent<PlanetoidEngine::CircleCollider2D>())
    {
        PlanetoidEngine::CircleCollider2D& circleCollider2D = entity.GetComponent<PlanetoidEngine::CircleCollider2D>();

        float radius = circleCollider2D.radius;
        glm::vec2 offset = circleCollider2D.offset;

        float density = circleCollider2D.density;
        float friction = circleCollider2D.friction;
        float restitution = circleCollider2D.restitution;
        int scaleWithTransform = (int)circleCollider2D.scaleWithTransform;

        sceneFile.AppendTextContents("CircleCollider2D:" + std::to_string(radius) + "," + std::to_string(offset.x) + "," + std::to_string(offset.y) + "," + std::to_string(density) + "," + std::to_string(friction) + "," + std::to_string(restitution) + "," + std::to_string(scaleWithTransform) + "\n");
    }
}

void Starship::InitEditorCamera()
{
    m_editorContext.editorCameraEntity = sceneManager.GetCurrentScene()->CreateEntity("EditorCamera");
    m_editorContext.editorCameraTransform = m_editorContext.editorCameraEntity.AddComponent<PlanetoidEngine::Transform>();
    m_editorContext.editorCameraComponent = m_editorContext.editorCameraEntity.AddComponent<PlanetoidEngine::Camera>();
    m_editorContext.editorCameraComponent.SetProjection(0.0f, 1280.0f, 0.0f, 720.0f);
    m_editorContext.editorCameraEntity.AddComponent<EditorExcludeComponent>();
}

bool Starship::TryLoadScriptDLL()
{
    if (m_editorContext.projectContext.dll)
    {
        PlanetoidEngine::Log::Warn("DLL already loaded");
    }

    if (m_editorContext.projectContext.rootPath == "")
    {
        PlanetoidEngine::Log::Error("No project loaded");
        return false;
    }

    if (!vfs.IsMounted("CodeProjectRoot"))
    {
        PlanetoidEngine::Log::Error("Code project not mounted");
        return false;
    }

    std::string dllPath = "CodeProjectRoot/bin/libPlanetoidEngine-" + m_editorContext.projectContext.name + "-ScriptEngine.dll";
    std::string dllPhysicalPath = vfs.ResolveVirtualPath(dllPath);

    if (!PlanetoidEngine::FileIO::FileExists(dllPhysicalPath))
    {
        PlanetoidEngine::Log::Error("DLL does not exist");
        return false;
    }

    std::string dllCopyPath = "CodeProjectRoot/bin/libPlanetoidEngine-" + m_editorContext.projectContext.name + "-ScriptEngine-Copy.dll";
    std::string dllCopyPhysicalPath = vfs.ResolveVirtualPath(dllCopyPath);

    if (PlanetoidEngine::FileIO::FileExists(dllCopyPhysicalPath))
    {
        PlanetoidEngine::FileIO::FileDelete(dllCopyPhysicalPath);
    }

    PlanetoidEngine::FileIO::FileCopy(dllPhysicalPath, dllCopyPhysicalPath);

    m_editorContext.projectContext.dll = LoadLibrary(dllCopyPhysicalPath.c_str());
    if (!m_editorContext.projectContext.dll)
    {
        PlanetoidEngine::Log::Error("Failed to load DLL");
        return false;
    }

    m_editorContext.projectContext.OnAttachFunc = (ProjectContext::ScriptDLLAttachFunc)GetProcAddress(m_editorContext.projectContext.dll, "OnAttach");
    if (!m_editorContext.projectContext.OnAttachFunc)
    {
        PlanetoidEngine::Log::Error("Failed to get OnAttach function");
        FreeLibrary(m_editorContext.projectContext.dll);
        return false;
    }

    m_editorContext.projectContext.OnDetachFunc = (ProjectContext::ScriptDLLDetachFunc)GetProcAddress(m_editorContext.projectContext.dll, "OnDetach");
    if (!m_editorContext.projectContext.OnDetachFunc)
    {
        PlanetoidEngine::Log::Error("Failed to get OnDetach function");
        FreeLibrary(m_editorContext.projectContext.dll);
        return false;
    }

    m_editorContext.projectContext.GetScriptFunc = (ProjectContext::GetScriptDLLFunc)GetProcAddress(m_editorContext.projectContext.dll, "GetScript");
    if (!m_editorContext.projectContext.GetScriptFunc)
    {
        PlanetoidEngine::Log::Error("Failed to get GetScript function");
        FreeLibrary(m_editorContext.projectContext.dll);
        return false;
    }

    m_editorContext.projectContext.GetScriptNamesFunc = (ProjectContext::GetScriptNamesDLLFunc)GetProcAddress(m_editorContext.projectContext.dll, "GetRegisteredScriptNames");
    if (!m_editorContext.projectContext.GetScriptNamesFunc)
    {
        PlanetoidEngine::Log::Error("Failed to get GetRegisteredScriptNames function");
        FreeLibrary(m_editorContext.projectContext.dll);
        return false;
    }

    PlanetoidEngine::Log::Success("Loaded Script Engine DLL");
    std::lock_guard<std::mutex> lock(m_vfsMutex);
    vfs.WatchFile(dllPhysicalPath);
    if (!m_fileChangedDelegateIterator.IsBound())
        m_fileChangedDelegateIterator = vfs.OnWatchedFileChanged.Bind(&Starship::OnDLLChanged, this);

    return true;
}

void Starship::OnDLLChanged(const std::string& path)
{
    std::string dllPath = "CodeProjectRoot/bin/libPlanetoidEngine-" + m_editorContext.projectContext.name + "-ScriptEngine.dll";
    std::string dllPhysicalPath = vfs.ResolveVirtualPath(dllPath);

    if (path != dllPhysicalPath)
    {
        return;
    }

    DetachEntityScripts();

    PlanetoidEngine::Log::Info("DLL changed");
    if (m_editorContext.projectContext.dll)
    {
        m_editorContext.projectContext.OnDetachFunc();
        FreeLibrary(m_editorContext.projectContext.dll);
        m_editorContext.projectContext.dll = nullptr;
    }


    TryLoadScriptDLL();

    AttachEntityScripts();
}

// Editor

// TODO: Play/Pause/Stop functionality
void Starship::OnPlay()
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    if (!scene || m_editorContext.projectContext.currentSceneName == "")
    {
        PlanetoidEngine::Log::Error("No Scene Loaded!");
        return;
    }

    if (m_editorContext.playState == EditorContext::PlayState::Paused)
    {
        scene->SetPaused(false);
        m_editorContext.playState = EditorContext::PlayState::Playing;
        return;
    }

    if (!m_editorContext.projectContext.dll)
    {
        TryLoadScriptDLL();
        if (!m_editorContext.projectContext.dll)
        {
            PlanetoidEngine::Log::Error("Failed to compile scripts");
            return;
        }
    }

    CreateSceneForPlayInEditor();

    std::vector<PlanetoidEngine::Entity> entities = scene->GetAllEntities();
    bool validCameraFound = false;

    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i].HasComponent<PlanetoidEngine::Camera>() && !entities[i].HasComponent<EditorExcludeComponent>())
        {
            validCameraFound = true;
            m_editorContext.playInEditorCameraTransform = entities[i].GetComponent<PlanetoidEngine::Transform>();
            m_editorContext.playInEditorCameraComponent = entities[i].GetComponent<PlanetoidEngine::Camera>();
            break;
        }
    }

    if (!validCameraFound)
    {
        PlanetoidEngine::Log::Error("No Camera Found!");

        std::string playSceneName = sceneManager.GetCurrentSceneName();
        sceneManager.DestroyScene(playSceneName);

        sceneManager.SetCurrentScene(m_editorContext.projectContext.currentSceneName);

        return;
    }

    m_EnableFileWatcher.store(false);
    if (fileWatcherThread.joinable())
    {
        fileWatcherThread.join();
    }

    m_editorContext.playState = EditorContext::PlayState::Playing;

    m_editorContext.selectedEntity = PlanetoidEngine::Entity();
    m_editorContext.selectedEntityComponentIndex = -1;
    
    AttachEntityScripts();
    sceneManager.GetCurrentScene()->OnLoad();
}

void Starship::OnPause()
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    if (!scene)
    {
        PlanetoidEngine::Log::Error("No Scene Loaded!");
        m_editorContext.playState = EditorContext::PlayState::Stopped;
        return;
    }

    scene->SetPaused(true);
    m_editorContext.playState = EditorContext::PlayState::Paused;
}

void Starship::OnStop()
{
    PlanetoidEngine::InputManager::ClearInputs();
    PlanetoidEngine::InputManager::OnScrollWheel.UnbindAll();
    PlanetoidEngine::InputManager::OnFileDrop.UnbindAll();

    DetachEntityScripts();

    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    scene->OnUnload();

    // Delete the play scenes
    for (auto it = m_editorContext.playInEditorSceneCopies.begin(); it != m_editorContext.playInEditorSceneCopies.end(); it++)
    {
        sceneManager.DestroyScene(it->second);
    }
    m_editorContext.playInEditorSceneCopies.clear();

    //std::string playSceneName = sceneManager.GetCurrentSceneName();
    //sceneManager.DestroyScene(playSceneName);

    m_editorContext.selectedEntity = PlanetoidEngine::Entity();
    m_editorContext.selectedEntityComponentIndex = -1;

    m_editorContext.playState = EditorContext::PlayState::Stopped;

    m_EnableFileWatcher.store(true);
    fileWatcherThread = std::thread(&Starship::FileWatcherTask, this);

    sceneManager.SetCurrentScene(m_editorContext.projectContext.currentSceneName);    
}

void Starship::AttachEntityScripts(bool callOnAttach)
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    if (!scene)
    {
        PlanetoidEngine::Log::Error("No Scene Loaded!");
        return;
    }

    if (!m_editorContext.projectContext.dll)
    {
        PlanetoidEngine::Log::Error("No DLL Loaded!");
        return;
    }

    if (!m_editorContext.projectContext.OnAttachFunc)
    {
        PlanetoidEngine::Log::Error("No OnAttach Function!");
        return;
    }
    m_editorContext.projectContext.OnAttachFunc();

    if (!m_editorContext.projectContext.OnDetachFunc)
    {
        PlanetoidEngine::Log::Error("No OnDetach Function!");
        return;
    }

    if (!m_editorContext.projectContext.GetScriptFunc)
    {
        PlanetoidEngine::Log::Error("No GetScript Function!");
        return;
    }

    std::vector<PlanetoidEngine::Entity> entities = scene->GetAllEntities();
    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i].HasComponent<PlanetoidEngine::ScriptComponent>())
        {
            PlanetoidEngine::ScriptComponent& scriptComponent = entities[i].GetComponent<PlanetoidEngine::ScriptComponent>();

            if (scriptComponent.scriptPath == "")
            {
                continue;
            }

            scriptComponent.script = m_editorContext.projectContext.GetScriptFunc(scriptComponent.scriptPath);
            if (scriptComponent.script)
            {
                if (callOnAttach)
                    scriptComponent.script->OnAttach(entities[i]);
            }
            else
            {
                std::string errorMsg = "Failed to get script " + scriptComponent.scriptPath + " from DLL for Entity " + entities[i].GetComponent<PlanetoidEngine::NameComponent>().name;
                PlanetoidEngine::Log::Error(errorMsg);
            }
        }
    }
}

void Starship::DetachEntityScripts(bool callOnDetach)
{
    PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
    if (!scene)
    {
        PlanetoidEngine::Log::Error("No Scene Loaded!");
        return;
    }

    if (!m_editorContext.projectContext.dll)
    {
        PlanetoidEngine::Log::Error("No DLL Loaded!");
        return;
    }

    if (!m_editorContext.projectContext.OnDetachFunc)
    {
        PlanetoidEngine::Log::Error("No OnDetach Function!");
        return;
    }
    m_editorContext.projectContext.OnDetachFunc();

    std::vector<PlanetoidEngine::Entity> entities = scene->GetAllEntities();
    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i].HasComponent<PlanetoidEngine::ScriptComponent>())
        {
            PlanetoidEngine::ScriptComponent& scriptComponent = entities[i].GetComponent<PlanetoidEngine::ScriptComponent>();
            if (scriptComponent.script)
            {
                if (callOnDetach)
                    scriptComponent.script->OnDetach();
                
                delete scriptComponent.script;
                scriptComponent.script = nullptr;
            }
        }
    }
}

void Starship::ProjectWindow()
{
    if (m_editorContext.showProjectWindow)
    {
        ImGui::OpenPopup("Project Window");
        m_editorContext.showProjectWindow = false;
    }

    if (ImGui::BeginPopupModal("Project Window"))
    {
        if (m_editorContext.closeProjectWindow)
        {
            ImGui::CloseCurrentPopup();
            m_editorContext.closeProjectWindow = false;
        }

        ImGui::Columns(3);
        if (ImGui::Button("New Project"))
        {
            m_editorContext.showNewProjectPopup = true;
        }
        ImGui::NextColumn();
        if (ImGui::Button("Open Project"))
        {
            OpenProjectPopup();
        }
        ImGui::NextColumn();
        ImGui::Text("Recent Projects");

        for (int i = 0; i < m_editorContext.recentProjects.size(); i++)
        {
            // Trim the path to just the project name
            std::string projectPath = m_editorContext.recentProjects[i];
            std::string projectName = projectPath.substr(projectPath.find_last_of("/") + 1);
            // Trim the extension
            projectName = projectName.substr(0, projectName.find_last_of("."));
        
            if (ImGui::Button(projectName.c_str()))
            {
                if (LoadProject(projectPath))
                {
                    // Move the project to the top of the list
                    m_editorContext.recentProjects.erase(m_editorContext.recentProjects.begin() + i);
                    m_editorContext.recentProjects.insert(m_editorContext.recentProjects.begin(), projectPath);
                    WriteRecentProjects();
                    std::string title = "Starship Editor " + m_editorContext.GetVersionString() + " - " + projectName;
                    window.SetWindowTitle(title.c_str());
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        ImGui::Columns(1);
        ImGui::Separator();
        if (ImGui::Button("Exit"))
        {
            Exit();
            ImGui::End();
            return;
        }

        ImGui::EndPopup();
    }
}

void Starship::NewProjectPopup()
{
    if (m_editorContext.showNewProjectPopup)
    {
        ImGui::OpenPopup("New Project");
        m_editorContext.showNewProjectPopup = false;
    }

    if (ImGui::BeginPopupModal("New Project"))
    {
        static char projectName[32] = "Untitled";
        static char projectPath[256] = "";
        ImGui::InputText("Project Name", projectName, 32);
        ImGui::InputText("Project Path", projectPath, 256);
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            std::string projectPathStr = PlanetoidEngine::FileIO::PickFolder("Select Project Folder");
            if (projectPath != "")
            {
                strcpy_s(projectPath, projectPathStr.c_str());
            }
        }
        ImGui::Separator();
        if (ImGui::Button("Create"))
        {
            std::string projectPathStr = projectPath;
            
            if (CreateNewBlankProject(projectName, projectPath))
            {
                m_editorContext.recentProjects.insert(m_editorContext.recentProjects.begin(), projectPathStr + "/" + std::string(projectName) + "/" + std::string(projectName) + ".ship");
                WriteRecentProjects();

                std::string title = "Starship Editor " + m_editorContext.GetVersionString() + " - " + projectName;
                window.SetWindowTitle(title.c_str());

                ImGui::CloseCurrentPopup();
                m_editorContext.closeProjectWindow = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            if (m_editorContext.projectContext.rootPath == "")
            {
                m_editorContext.showProjectWindow = true;
            }
        }
        ImGui::EndPopup();
    }
}

void Starship::OpenProjectPopup()
{
    std::string projectPathStr = PlanetoidEngine::FileIO::PickFile("Select Project File", "Starship Project", "*.ship");
    if (projectPathStr == "")
    {
        return;
    }

    if (LoadProject(projectPathStr))
    {

        // Get the project name from the path
        std::string projectName = projectPathStr.substr(projectPathStr.find_last_of("/") + 1);
        // Trim the extension
        projectName = projectName.substr(0, projectName.find_last_of("."));

        // Remove the project from the recent projects list if it already exists
        if (m_editorContext.recentProjects.size() > 0)
        {
            for (int i = 0; i < m_editorContext.recentProjects.size(); i++)
            {
                if (m_editorContext.recentProjects[i] == projectPathStr)
                {
                    m_editorContext.recentProjects.erase(m_editorContext.recentProjects.begin() + i);
                    break;
                }
            }
        }

        m_editorContext.recentProjects.insert(m_editorContext.recentProjects.begin(), projectPathStr);
        WriteRecentProjects();

        std::string title = "Starship Editor " + m_editorContext.GetVersionString() + " - " + projectName;
        window.SetWindowTitle(title.c_str());

        m_editorContext.closeProjectWindow = true;
    }
}

void Starship::RenameEntityPopup()
{
    if (m_editorContext.showRenameEntityPopup)
    {
        ImGui::OpenPopup("Rename Entity");
        m_editorContext.showRenameEntityPopup = false;
    }

    if (ImGui::BeginPopupModal("Rename Entity"))
    {
        static char entityName[32] = "";
        ImGui::InputText("Entity Name", entityName, 32);
        if (ImGui::Button("Rename"))
        {
            std::string previousName = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::NameComponent>().name;
            m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::NameComponent>().name = entityName;
            PlanetoidEngine::Log::Info("Renamed Entity " + previousName + " to " + entityName);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Starship::NewScenePopup()
{
    if (m_editorContext.showNewScenePopup)
    {
        ImGui::OpenPopup("New Scene");
        m_editorContext.showNewScenePopup = false;
    }

    if (ImGui::BeginPopupModal("New Scene"))
    {
        static char sceneName[32] = "Untitled";
        ImGui::InputText("Scene Name", sceneName, 32);
        if (ImGui::Button("Create"))
        {
            if (CreateNewBlankScene(sceneName))
            {
                LoadScene(sceneName);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Starship::OpenScenePopup()
{
    if (m_editorContext.showOpenScenePopup)
    {
        ImGui::OpenPopup("Open Scene");
        m_editorContext.showOpenScenePopup = false;
    }

    if (ImGui::BeginPopupModal("Open Scene"))
    {
        std::vector<std::string> scenes = vfs.GetFolders("Scenes");
        for (int i = 0; i < scenes.size(); i++)
        {
            if (ImGui::Button(scenes[i].c_str()))
            {
                if (LoadScene(scenes[i]))
                {
                    sceneManager.SetCurrentScene(scenes[i]);
                    m_editorContext.projectContext.currentSceneName = scenes[i];
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::EndPopup();
    }
}

void Starship::SceneSettingsPopup()
{
    if (m_editorContext.showSceneSettingsPopup)
    {
        ImGui::OpenPopup("Scene Settings");
        m_editorContext.showSceneSettingsPopup = false;
    }

    if (ImGui::BeginPopupModal("Scene Settings"))
    {
        PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
        if (!scene)
        {
            ImGui::Text("No Scene Loaded");
            ImGui::CloseCurrentPopup();
            return;
        }

        ImGui::EndPopup();
    }
}

void Starship::NewFolderPopup()
{
    if (m_editorContext.showNewFolderPopup)
    {
        ImGui::OpenPopup("New Folder");
        m_editorContext.showNewFolderPopup = false;
    }

    if (ImGui::BeginPopupModal("New Folder"))
    {
        static char folderName[32] = "Untitled";
        ImGui::InputText("Folder Name", folderName, 32);
        if (ImGui::Button("Create"))
        {
            std::string folderNameStr = folderName;
            std::string folderVirtualPath = m_editorContext.assetBrowserSelectedPath + "/" + folderNameStr;
            std::string folderPhysicalPath = vfs.ResolveVirtualPath(folderVirtualPath);
            PlanetoidEngine::FileIO::FolderCreate(folderPhysicalPath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Starship::NewScriptPopup()
{
    if (m_editorContext.showNewScriptPopup)
    {
        ImGui::OpenPopup("New Script");
        m_editorContext.showNewScriptPopup = false;
    }

    if (ImGui::BeginPopupModal("New Script"))
    {
        static char scriptName[32] = "Untitled";
        ImGui::InputText("Script Name", scriptName, 32);
        if (ImGui::Button("Create"))
        {
            std::string scriptNameStr = scriptName;
            std::string scriptVirtualPath = m_editorContext.assetBrowserSelectedPath + "/" + scriptNameStr + ".h";
            std::string scriptPhysicalPath = vfs.ResolveVirtualPath(scriptVirtualPath);
            PlanetoidEngine::FileIO::FileCreate(scriptPhysicalPath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Starship::MainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Project"))
            {
                m_editorContext.showNewProjectPopup = true;
            }
            if (ImGui::MenuItem("Open Project"))
            {
                OpenProjectPopup();
            }
            if (ImGui::MenuItem("Save Project"))
            {
                SaveScene();
                SaveProject();
            }
            if (ImGui::BeginMenu("Recent Projects"))
            {
                for (int i = 0; i < m_editorContext.recentProjects.size(); i++)
                {
                    // Trim the path to just the project name
                    std::string projectPath = m_editorContext.recentProjects[i];
                    std::string projectName = projectPath.substr(projectPath.find_last_of("/") + 1);
                    // Trim the extension
                    projectName = projectName.substr(0, projectName.find_last_of("."));

                    if (ImGui::MenuItem(projectName.c_str()))
                    {
                        if (LoadProject(projectPath))
                        {
                            m_editorContext.recentProjects.erase(m_editorContext.recentProjects.begin() + i);
                            m_editorContext.recentProjects.insert(m_editorContext.recentProjects.begin(), projectPath);
                            WriteRecentProjects();
                            std::string title = "Starship Editor " + m_editorContext.GetVersionString() + " - " + projectName;
                            window.SetWindowTitle(title.c_str());
                        }
                    }
                
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();

            if (ImGui::MenuItem("New Scene"))
            {
                m_editorContext.showNewScenePopup = true;
            }

            if (ImGui::MenuItem("Open Scene"))
            {
                m_editorContext.showOpenScenePopup = true;
            }

            if (ImGui::MenuItem("Save Scene"))
            {
                SaveScene();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Build Project"))
            {

            }

            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                Exit();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Project Settings"))
            {
                
            }
            if (ImGui::MenuItem("Editor Preferences"))
            {

            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                m_editorContext.showAboutWindow = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Starship::AboutWindow()
{
    if (!m_editorContext.showAboutWindow)
    {
        return;
    }
    if (ImGui::Begin("About"))
    {
        ImGui::Text("Starship Editor");
        ImGui::Text("Version: %s", m_editorContext.GetVersionString().c_str());
        ImGui::Separator();
        ImGui::Text("Planetoid Engine");
        ImGui::Text("Version: %s", PlanetoidEngine::GetVersionString().c_str());
        if (ImGui::Button("Close"))
        {
            m_editorContext.showAboutWindow = false;
        }
    }
    ImGui::End();
}

void Starship::MainDockSpace()
{
    m_editorContext.dockspaceID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

static bool PointInQuad(const glm::vec2& p, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4)
{
    auto edge1 = v2 - v1;
    auto edge2 = v3 - v2;
    auto edge3 = v4 - v3;
    auto edge4 = v1 - v4;

    auto normal1 = glm::vec2(-edge1.y, edge1.x);
    auto normal2 = glm::vec2(-edge2.y, edge2.x);
    auto normal3 = glm::vec2(-edge3.y, edge3.x);
    auto normal4 = glm::vec2(-edge4.y, edge4.x);

    return (glm::dot(p - v1, normal1) >= 0.0f &&
            glm::dot(p - v2, normal2) >= 0.0f &&
            glm::dot(p - v3, normal3) >= 0.0f &&
            glm::dot(p - v4, normal4) >= 0.0f);
}

void Starship::Viewport()
{
    if (ImGui::Begin("Viewport"))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x != m_editorContext.viewportSize.x || viewportSize.y != m_editorContext.viewportSize.y)
        {
            m_editorContext.viewportSize = glm::vec2(viewportSize.x, viewportSize.y);
        }
        ImGui::Image(reinterpret_cast<void*>(m_editorContext.framebuffer.GetColorAttachment().GetHandle()), ImVec2(viewportSize.x, viewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

        if (m_editorContext.playState != EditorContext::PlayState::Stopped)
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Scene"))
            {
                std::string assetPath = std::string((char*)payload->Data);
                LoadScene(assetPath);
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemHovered())
        {
            m_editorContext.editorCameraControls = true;
            // Get the mouse position relative to the viewport
            glm::vec2 mousePos = {ImGui::GetMousePos().x, ImGui::GetMousePos().y};
            mousePos.x -= ImGui::GetItemRectMin().x;
            mousePos.y -= ImGui::GetItemRectMin().y;

            // Convert the mouse position to a position in the world
            mousePos.x = (mousePos.x / viewportSize.x) * 1280.0f;
            mousePos.y = (mousePos.y / viewportSize.y) * 720.0f;

            mousePos.y = 720.0f - mousePos.y;

            // Apply the camera transform to the mouse position
            glm::mat4 invCameraTransform = glm::inverse(m_editorContext.editorCameraTransform.GetMatrix());
            mousePos = glm::vec2(invCameraTransform * glm::vec4(mousePos.x, mousePos.y, 0.0f, 1.0f));


            static glm::vec2 rp1;
            static glm::vec2 rp2;
            static glm::vec2 rp3;
            static glm::vec2 rp4;

            // Get all entities with a transform and sprite renderer
            PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
            if (scene)
            {
                bool clicked = false;
                std::vector<PlanetoidEngine::Entity> entities;
                scene->GetRegistry().view<PlanetoidEngine::UUID, PlanetoidEngine::NameComponent, PlanetoidEngine::Transform, PlanetoidEngine::SpriteRenderer>().each([&](auto& entityUUID, auto& name, auto& transform, auto& spriteRenderer)
                {
                    glm::vec2 position = transform.GetPosition();
                    glm::vec2 scale = transform.GetScale();
                    float rotation = transform.GetRotation();

                    glm::mat4 transformMatrix = transform.GetMatrix();

                    // Vertices of the entity's quad in local space
                    glm::vec2 localP1 = glm::vec2(-0.5f, -0.5f);
                    glm::vec2 localP2 = glm::vec2(0.5f, -0.5f);
                    glm::vec2 localP3 = glm::vec2(0.5f, 0.5f);
                    glm::vec2 localP4 = glm::vec2(-0.5f, 0.5f);

                    // Rotate and translate each point
                    glm::vec2 rotatedP1 = transformMatrix * glm::vec4(localP1, 0.0f, 1.0f);
                    glm::vec2 rotatedP2 = transformMatrix * glm::vec4(localP2, 0.0f, 1.0f);
                    glm::vec2 rotatedP3 = transformMatrix * glm::vec4(localP3, 0.0f, 1.0f);
                    glm::vec2 rotatedP4 = transformMatrix * glm::vec4(localP4, 0.0f, 1.0f);

                    // Check for mouse interaction
                    if (PointInQuad(mousePos, rotatedP1, rotatedP2, rotatedP3, rotatedP4))
                    {
                        entities.push_back(scene->GetEntity(entityUUID));

                        if (ImGui::IsItemClicked())
                        {
                            clicked = true;
                            rp1 = rotatedP1;
                            rp2 = rotatedP2;
                            rp3 = rotatedP3;
                            rp4 = rotatedP4;
                        }
                    }
                });

                if (entities.size() == 0)
                {
                    ImGui::End();
                    return;
                }

                float depth = 3.0f;
                PlanetoidEngine::Entity selectedEntity;
                for (int i = 0; i < entities.size(); i++)
                {
                    PlanetoidEngine::Entity entity = entities[i];
                    PlanetoidEngine::Transform& transform = entity.GetComponent<PlanetoidEngine::Transform>();
                    glm::vec3 position = transform.GetPosition();
                    

                    if (position.z < depth)
                    {
                        depth = position.z;
                        selectedEntity = entity;
                    }
                }

                PlanetoidEngine::Transform& transform = selectedEntity.GetComponent<PlanetoidEngine::Transform>();
                PlanetoidEngine::NameComponent& name = selectedEntity.GetComponent<PlanetoidEngine::NameComponent>();
                PlanetoidEngine::SpriteRenderer& spriteRenderer = selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>();

                glm::vec3 position = transform.GetPosition();
                glm::vec2 scale = transform.GetScale();
                float rotation = transform.GetRotation();

                // Draw the oriented bounding box (OBB)
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->PushClipRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true);

                glm::mat4 transformMatrix = transform.GetMatrix();
                glm::vec2 itemRectMin = {ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y};

                // Vertices of the entity's quad in local space
                glm::vec2 localP1 = glm::vec2(-0.5f, -0.5f);
                glm::vec2 localP2 = glm::vec2(0.5f, -0.5f);
                glm::vec2 localP3 = glm::vec2(0.5f, 0.5f);
                glm::vec2 localP4 = glm::vec2(-0.5f, 0.5f);

                // Rotate and translate each point
                glm::vec2 rotatedP1 = glm::vec2(transformMatrix * glm::vec4(localP1, 0.0f, 1.0f)) + glm::vec2(m_editorContext.editorCameraTransform.GetPosition());
                glm::vec2 rotatedP2 = glm::vec2(transformMatrix * glm::vec4(localP2, 0.0f, 1.0f)) + glm::vec2(m_editorContext.editorCameraTransform.GetPosition());
                glm::vec2 rotatedP3 = glm::vec2(transformMatrix * glm::vec4(localP3, 0.0f, 1.0f)) + glm::vec2(m_editorContext.editorCameraTransform.GetPosition());
                glm::vec2 rotatedP4 = glm::vec2(transformMatrix * glm::vec4(localP4, 0.0f, 1.0f)) + glm::vec2(m_editorContext.editorCameraTransform.GetPosition());

                rotatedP1 = glm::vec2((rotatedP1.x / 1280.0f) * viewportSize.x, viewportSize.y - (rotatedP1.y / 720.0f) * viewportSize.y);
                rotatedP2 = glm::vec2((rotatedP2.x / 1280.0f) * viewportSize.x, viewportSize.y - (rotatedP2.y / 720.0f) * viewportSize.y);
                rotatedP3 = glm::vec2((rotatedP3.x / 1280.0f) * viewportSize.x, viewportSize.y - (rotatedP3.y / 720.0f) * viewportSize.y);
                rotatedP4 = glm::vec2((rotatedP4.x / 1280.0f) * viewportSize.x, viewportSize.y - (rotatedP4.y / 720.0f) * viewportSize.y);

                rotatedP1 += itemRectMin;
                rotatedP2 += itemRectMin;
                rotatedP3 += itemRectMin;
                rotatedP4 += itemRectMin;

                // Draw the OBB
                drawList->AddQuad(ImVec2(rotatedP1.x, rotatedP1.y), ImVec2(rotatedP2.x, rotatedP2.y), ImVec2(rotatedP3.x, rotatedP3.y), ImVec2(rotatedP4.x, rotatedP4.y), IM_COL32(255, 255, 255, 255));

                drawList->PopClipRect();
                // Clip drawing to the viewport

                ImGui::BeginTooltip();
                ImGui::Text("Entity: %s - %s", name.name.c_str(), selectedEntity.GetComponent<PlanetoidEngine::UUID>().ToString().c_str());
                ImGui::Text("Position: %f, %f, %f", position.x, position.y, position.z);
                ImGui::Text("Scale: %f, %f", scale.x, scale.y);
                ImGui::Text("Rotation: %f", rotation);
                ImGui::Text("Color: %f, %f, %f, %f", spriteRenderer.color.r, spriteRenderer.color.g, spriteRenderer.color.b, spriteRenderer.color.a);
                ImGui::EndTooltip();

                if (clicked)
                {
                    m_editorContext.selectedEntity = selectedEntity;
                    m_editorContext.selectedEntityComponents.clear();
                    m_editorContext.selectedEntityComponentIndex = 0;
                }
            }
        }
        else
        {
            m_editorContext.editorCameraControls = false;
        }
    }
    ImGui::End();
}

void Starship::EntityList()
{
    if (ImGui::Begin("Entities"))
    {
        PlanetoidEngine::Scene* scene = sceneManager.GetCurrentScene();
        if (scene == nullptr || m_editorContext.playState != EditorContext::PlayState::Stopped)
        {
            m_editorContext.selectedEntity = PlanetoidEngine::Entity();
            m_editorContext.selectedEntityComponents.clear();
            m_editorContext.selectedEntityComponentIndex = -1;

            ImGui::End();
            return;
        }

        if (ImGui::Button("Create Entity"))
        {
            ImGui::OpenPopup("Create Entity");
        }

        if (ImGui::BeginPopup("Create Entity"))
        {
            static char entityName[32] = "Entity";
            ImGui::InputText("Entity Name", entityName, 32);
            if (ImGui::Button("Create"))
            {
                scene->CreateEntity(entityName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        std::vector<PlanetoidEngine::Entity> entities = scene->GetAllEntities();

        if (ImGui::BeginListBox("##Entities", ImVec2(-FLT_MIN, -FLT_MIN)))
        {
            for (int i = 0; i < entities.size(); i++)
            {
                PlanetoidEngine::Entity entity = entities[i];
                PlanetoidEngine::UUID uuid = entity.GetComponent<PlanetoidEngine::UUID>();
                std::string name = entity.GetComponent<PlanetoidEngine::NameComponent>().name + "##" + uuid.ToString();

                if (entity.HasComponent<EditorExcludeComponent>())
                {
                    continue;
                }

                if (ImGui::Selectable(name.c_str(), m_editorContext.selectedEntity == entity))
                {
                    m_editorContext.selectedEntity = entity;

                    m_editorContext.selectedEntityComponents.clear();
                    if (entity.HasComponent<PlanetoidEngine::Transform>())
                    {
                        m_editorContext.selectedEntityComponents.push_back("Transform");
                    }
                    if (entity.HasComponent<PlanetoidEngine::SpriteRenderer>())
                    {
                        m_editorContext.selectedEntityComponents.push_back("Sprite Renderer");
                    }
                    if (entity.HasComponent<PlanetoidEngine::Camera>())
                    {
                        m_editorContext.selectedEntityComponents.push_back("Camera");
                    }
                    m_editorContext.selectedEntityComponentIndex = (m_editorContext.selectedEntityComponents.size() > 0 ? 0 : -1);
                }

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete Entity"))
                    {
                        scene->DestroyEntity(entity);
                        m_editorContext.selectedEntity = PlanetoidEngine::Entity();
                        m_editorContext.selectedEntityComponents.clear();
                        m_editorContext.selectedEntityComponentIndex = -1;
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::MenuItem("Rename Entity"))
                    {
                        m_editorContext.showRenameEntityPopup = true;
                        ImGui::CloseCurrentPopup();
                    }
                    if (m_editorContext.selectedEntity != PlanetoidEngine::Entity())
                    {
                        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Transform>())
                        {
                            if (ImGui::MenuItem("Focus on Entity"))
                            {
                                PlanetoidEngine::Transform& transform = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::Transform>();
                                glm::vec3 position = transform.GetPosition();
                                position.x -= m_editorContext.editorCameraComponent.right / 2.0f;
                                position.y -= m_editorContext.editorCameraComponent.top / 2.0f;
                                m_editorContext.editorCameraTransform.SetPosition(-position);
                            }
                        }
                    }
                    
                    ImGui::EndPopup();
                }
            }
            ImGui::EndListBox();
        }
    }
    ImGui::End();
}

void Starship::ComponentList()
{
    if (ImGui::Begin("Component List"))
    {
        if (m_editorContext.selectedEntity == PlanetoidEngine::Entity())
        {
            ImGui::End();
            return;
        }

        PlanetoidEngine::NameComponent& name = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::NameComponent>();
        ImGui::Text("Entity: %s - %s", name.name.c_str(), m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::UUID>().ToString().c_str());
        if (ImGui::Button("Copy Entity ID"))
        {
            ImGui::SetClipboardText(m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::UUID>().ToString().c_str());
        }

        m_editorContext.selectedEntityComponents.clear();
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Transform>())
        {
            m_editorContext.selectedEntityComponents.push_back("Transform");
        }
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::SpriteRenderer>())
        {
            m_editorContext.selectedEntityComponents.push_back("Sprite Renderer");
        }
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Camera>())
        {
            m_editorContext.selectedEntityComponents.push_back("Camera");
        }
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::ScriptComponent>())
        {
            m_editorContext.selectedEntityComponents.push_back("Script");
        }
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::BoxCollider2D>())
        {
            m_editorContext.selectedEntityComponents.push_back("Box Collider 2D");
        }
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::CircleCollider2D>())
        {
            m_editorContext.selectedEntityComponents.push_back("Circle Collider 2D");
        }
        if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::RigidBody2D>())
        {
            m_editorContext.selectedEntityComponents.push_back("RigidBody 2D");
        }

        if (ImGui::BeginListBox("##Components"))
        {
            for (int i = 0; i < m_editorContext.selectedEntityComponents.size(); i++)
            {
                if (ImGui::Selectable(m_editorContext.selectedEntityComponents[i].c_str(), m_editorContext.selectedEntityComponentIndex == i))
                {
                    m_editorContext.selectedEntityComponentIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture"))
            {
                std::string assetPath = std::string((char*)payload->Data);
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::SpriteRenderer>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::SpriteRenderer>();
                }
                PlanetoidEngine::Texture* texture = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>().texture;
                texture->LoadFromFile(assetPath);
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("Add Component");
        }

        if (ImGui::BeginPopup("Add Component"))
        {
            if (ImGui::MenuItem("Transform"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Transform>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::Transform>();
                    m_editorContext.selectedEntityComponents.push_back("Transform");
                }
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Sprite Renderer"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::SpriteRenderer>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::SpriteRenderer>();
                    m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>().color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>().texture = new PlanetoidEngine::Texture();
                    m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>().texture->RandomNoise(12, 12, 4);
                    m_editorContext.selectedEntityComponents.push_back("Sprite Renderer");
                }
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Camera"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Camera>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::Camera>();
                    m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::Camera>().SetProjection(0.0f, 1280.0f, 0.0f, 720.0f);
                    m_editorContext.selectedEntityComponents.push_back("Camera");
                }
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Script"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::ScriptComponent>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::ScriptComponent>();
                    m_editorContext.selectedEntityComponents.push_back("Script");
                }
            }
            if (ImGui::MenuItem("Box Collider 2D"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::BoxCollider2D>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::BoxCollider2D>();
                    m_editorContext.selectedEntityComponents.push_back("Box Collider 2D");
                }
            }
            if (ImGui::MenuItem("Circle Collider 2D"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::CircleCollider2D>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::CircleCollider2D>();
                    m_editorContext.selectedEntityComponents.push_back("Circle Collider 2D");
                
                }
            }
            if (ImGui::MenuItem("RigidBody 2D"))
            {
                if (!m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::RigidBody2D>())
                {
                    m_editorContext.selectedEntity.AddComponent<PlanetoidEngine::RigidBody2D>();
                    m_editorContext.selectedEntityComponents.push_back("RigidBody 2D");
                }
            }   
            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove Component"))
        {
            if (m_editorContext.selectedEntityComponentIndex != -1 && m_editorContext.selectedEntityComponentIndex < m_editorContext.selectedEntityComponents.size())
            {
                if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "Transform")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::Transform>();
                }
                else if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "Sprite Renderer")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::SpriteRenderer>();
                }
                else if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "Camera")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::Camera>();
                }
                else if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "Script")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::ScriptComponent>();
                }
                else if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "Box Collider 2D")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::BoxCollider2D>();
                }
                else if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "Circle Collider 2D")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::CircleCollider2D>();
                }
                else if (m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex] == "RigidBody 2D")
                {
                    m_editorContext.selectedEntity.RemoveComponent<PlanetoidEngine::RigidBody2D>();
                }
                m_editorContext.selectedEntityComponentIndex = -1;
            }
        }

        ImGui::Separator();

        Inspector();
    }
    ImGui::End();
}

void Starship::Inspector()
{
    if (m_editorContext.selectedEntity == PlanetoidEngine::Entity())
    {
        return;
    }

    if (m_editorContext.selectedEntityComponentIndex == -1)
    {
        return;
    }

    std::string componentName = m_editorContext.selectedEntityComponents[m_editorContext.selectedEntityComponentIndex];
    if (componentName == "Transform")
    {
        InspectorTransform();
    }
    else if (componentName == "Sprite Renderer")
    {
        InspectorSpriteRenderer();
    }
    else if (componentName == "Camera")
    {
        InspectorCamera();
    }
    else if (componentName == "Script")
    {
        InspectorScript();
    }
    else if (componentName == "Box Collider 2D")
    {
        InspectorBoxCollider2D();
    }
    else if (componentName == "Circle Collider 2D")
    {
        InspectorCircleCollider2D();
    }
    else if (componentName == "RigidBody 2D")
    {
        InspectorRigidBody2D();
    }
}

void Starship::AssetBrowser()
{
    if (ImGui::Begin("Asset Browser"))
    {
        if (m_editorContext.projectContext.rootPath == "")
        {
            ImGui::End();
            return;
        }

        ImGuiChildFlags childflags = ImGuiChildFlags_Border;
        if (ImGui::BeginChild("Folder Tree", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, 0), childflags | ImGuiChildFlags_ResizeX, ImGuiWindowFlags_HorizontalScrollbar))
        {
            DrawFolderView();
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("Browser", ImVec2(ImGui::GetContentRegionAvail().x, 0), childflags, ImGuiWindowFlags_HorizontalScrollbar))
        {
            DrawBrowserView();
        }
        ImGui::EndChild();

    }
    ImGui::End();
}

void Starship::Console()
{
    if (ImGui::Begin("Console"))
    {
        if (ImGui::Button("Clear"))
        {
            m_editorContext.consoleMessages.clear();
        }

        ImGui::BeginChild("Console Output", ImVec2(0, 0), ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_HorizontalScrollbar);
        
        for (int i = 0; i < m_editorContext.consoleMessages.size(); i++)
        {
            PlanetoidEngine::LogLevel level = (PlanetoidEngine::LogLevel)m_editorContext.consoleMessages[i].first;
            std::string message = m_editorContext.consoleMessages[i].second;

            // Push the color for the log level
            if (level == PlanetoidEngine::LogLevel::Info)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            else if (level == PlanetoidEngine::LogLevel::Success)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }
            else if (level == PlanetoidEngine::LogLevel::Warn)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            }
            else if (level == PlanetoidEngine::LogLevel::Error)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            }

            ImGui::Text(message.c_str());

            ImGui::PopStyleColor();
        }

        if (m_editorContext.consoleScrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
            m_editorContext.consoleScrollToBottom = false;
        }

        ImGui::EndChild();

    }
    ImGui::End();
}

void Starship::Toolbar()
{
    if (ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize))
    {
        unsigned int buttonHandle = m_editorContext.playState == EditorContext::PlayState::Playing ? m_editorContext.pauseButtonTexture.GetHandle() : m_editorContext.playButtonTexture.GetHandle();
        if (ImGui::ImageButton(reinterpret_cast<void*>(buttonHandle), ImVec2(32, 32)))
        {
            if (m_editorContext.playState == EditorContext::PlayState::Playing)
            {
                OnPause();
            }
            else
            {
                OnPlay();
            }
        }
        ImGui::SameLine();
        if (m_editorContext.playState == EditorContext::PlayState::Playing || m_editorContext.playState == EditorContext::PlayState::Paused)
        {
            if (ImGui::ImageButton(reinterpret_cast<void*>(m_editorContext.stopButtonTexture.GetHandle()), ImVec2(32, 32)))
            {
                OnStop();
            }
        }
    }
    ImGui::End();
}

// Inspector Widgets

void Starship::InspectorTransform()
{
    ImGui::Text("Transform Component");

    PlanetoidEngine::Transform& transform = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::Transform>();

    glm::vec3 position = transform.GetPosition();
    glm::vec2 scale = transform.GetScale();
    float rotation = transform.GetRotation();

    if (ImGui::DragFloat3("Position", &position.x, 1.0f))
    {
        transform.SetPosition(position);
    }

    if (ImGui::DragFloat("Rotation", &rotation, 1.0f))
    {
        transform.SetRotation(rotation);
    }

    if (ImGui::DragFloat2("Scale", &scale.x, 1.0f))
    {
        transform.SetScale(scale);
    }

    if (m_editorContext.selectedEntity.HasComponent<PlanetoidEngine::Camera>())
    {
        if (ImGui::Button("Snap to Editor Camera Position"))
        {
            transform.SetPosition(m_editorContext.editorCameraTransform.GetPosition());
        }
    }
}

void Starship::InspectorSpriteRenderer()
{
    

    ImGui::Text("Sprite Renderer Component");

    PlanetoidEngine::SpriteRenderer& spriteRenderer = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>();

    glm::vec4 color = spriteRenderer.color;
    PlanetoidEngine::Texture* texture = spriteRenderer.texture;

    if (ImGui::ColorEdit4("Color", &color.x))
    {
        spriteRenderer.color = color;
    }

    if (ImGui::Button("Load Texture"))
    {
        std::string texturePath = PlanetoidEngine::FileIO::PickFile("Select Texture", "Image", "*.png");
        if (texturePath != "")
        {
            texture->LoadFromFile(texturePath);
        }
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture"))
        {
            std::string assetPath = std::string((char*)payload->Data);
            PlanetoidEngine::Texture* texture = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::SpriteRenderer>().texture;
            texture->LoadFromFile(assetPath);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    if (ImGui::Button("Random Noise"))
    {
        texture->RandomNoise(12, 12, 4);
    }

    ImGui::SameLine();

    if (ImGui::Button("Blank"))
    {
        texture->White(1, 1, 4);
    }
}

void Starship::InspectorCamera()
{
    ImGui::Text("Camera Component");

    PlanetoidEngine::Camera& camera = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::Camera>();

    glm::vec2 resolution = glm::vec2(camera.right, camera.top);
    float nearPlane = camera.nearPlane;
    float farPlane = camera.farPlane;

    if (ImGui::DragFloat2("Resolution", &resolution.x, 1.0f))
    {
        camera.SetProjection(0.0f, resolution.x, 0.0f, resolution.y, nearPlane, farPlane);
    }

    if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f))
    {
        camera.SetProjection(0.0f, resolution.x, 0.0f, resolution.y, nearPlane, farPlane);
    }

    if (ImGui::DragFloat("Far Plane", &farPlane, 0.1f))
    {
        camera.SetProjection(0.0f, resolution.x, 0.0f, resolution.y, nearPlane, farPlane);
    }
}

void Starship::InspectorScript()
{
    ImGui::Text("Script Component");

    PlanetoidEngine::ScriptComponent& scriptComponent = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::ScriptComponent>();

    if (!m_editorContext.projectContext.dll)
    {
        return;
    }

    std::vector<std::string> scriptNames = m_editorContext.projectContext.GetScriptNamesFunc();

    std::string scriptPath = scriptComponent.scriptPath;
    std::string previewValue = scriptPath == "" ? "Select Script" : scriptPath;

    if (ImGui::BeginCombo("##Scripts", previewValue.c_str()))
    {
        for (int i = 0; i < scriptNames.size(); i++)
        {
            if (ImGui::Selectable(scriptNames[i].c_str(), scriptPath == scriptNames[i]))
            {
                scriptPath = scriptNames[i];
            }

            if (scriptPath == scriptNames[i])
            {
                ImGui::SetItemDefaultFocus();
                scriptComponent.scriptPath = scriptPath;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit Script"))
    {
        
    }
}

void Starship::InspectorBoxCollider2D()
{
    ImGui::Text("Box Collider 2D");

    PlanetoidEngine::BoxCollider2D& boxCollider2D = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::BoxCollider2D>();

    glm::vec2 size = boxCollider2D.size;
    glm::vec2 offset = boxCollider2D.offset;
    float density = boxCollider2D.density;
    float friction = boxCollider2D.friction;
    float restitution = boxCollider2D.restitution;

    bool scaleWithTransform = boxCollider2D.scaleWithTransform;

    if (ImGui::DragFloat2("Size", &size.x, 0.1f))
    {
        boxCollider2D.size = size;
    }

    if (ImGui::DragFloat2("Offset", &offset.x, 0.1f))
    {
        boxCollider2D.offset = offset;
    }

    if (ImGui::DragFloat("Density", &density, 0.01f, 0.0f, 1.0f))
    {
        boxCollider2D.density = density;
    }

    if (ImGui::DragFloat("Friction", &friction, 0.01f, 0.0f, 1.0f))
    {
        boxCollider2D.friction = friction;
    }

    if (ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 5.0f))
    {
        boxCollider2D.restitution = restitution;
    }

    if (ImGui::Checkbox("Scale With Transform", &scaleWithTransform))
    {
        boxCollider2D.scaleWithTransform = scaleWithTransform;
    }
}

void Starship::InspectorCircleCollider2D()
{
    ImGui::Text("Circle Collider 2D");

    PlanetoidEngine::CircleCollider2D& circleCollider2D = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::CircleCollider2D>();

    float radius = circleCollider2D.radius;
    glm::vec2 offset = circleCollider2D.offset;
    float density = circleCollider2D.density;
    float friction = circleCollider2D.friction;
    float restitution = circleCollider2D.restitution;

    bool scaleWithTransform = circleCollider2D.scaleWithTransform;

    if (ImGui::DragFloat("Radius", &radius, 0.1f))
    {
        circleCollider2D.radius = radius;
    }

    if (ImGui::DragFloat2("Offset", &offset.x, 0.1f))
    {
        circleCollider2D.offset = offset;
    }

    if (ImGui::DragFloat("Density", &density, 0.01f, 0.0f, 1.0f))
    {
        circleCollider2D.density = density;
    }

    if (ImGui::DragFloat("Friction", &friction, 0.01f, 0.0f, 1.0f))
    {
        circleCollider2D.friction = friction;
    }

    if (ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f))
    {
        circleCollider2D.restitution = restitution;
    }

    if (ImGui::Checkbox("Scale With Transform", &scaleWithTransform))
    {
        circleCollider2D.scaleWithTransform = scaleWithTransform;
    }
}

void Starship::InspectorRigidBody2D()
{
    ImGui::Text("RigidBody 2D");

    PlanetoidEngine::RigidBody2D& rigidBody2D = m_editorContext.selectedEntity.GetComponent<PlanetoidEngine::RigidBody2D>();

    std::vector<std::string> bodyTypes = { "Static", "Dynamic", "Kinematic" };
    int bodyTypeIndex = (int)rigidBody2D.type;
    if (ImGui::BeginCombo("Body Type", bodyTypes[bodyTypeIndex].c_str()))
    {
        for (int i = 0; i < bodyTypes.size(); i++)
        {
            bool selected = bodyTypeIndex == i;
            if (ImGui::Selectable(bodyTypes[i].c_str(), selected))
            {
                bodyTypeIndex = i;
                rigidBody2D.type = (PlanetoidEngine::RigidBody2D::RigidBodyType)bodyTypeIndex;
            }
            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    bool allowRotation = rigidBody2D.allowRotation;
    if (ImGui::Checkbox("Allow Rotation", &allowRotation))
    {
        rigidBody2D.allowRotation = allowRotation;
    }
}

// Asset Browser Widgets

void Starship::DrawFolderView()
{
    ImGuiTreeNodeFlags flags = m_editorContext.assetBrowserTreeFlags;
    if (m_editorContext.assetBrowserSelectedPath == "GlobalAssets")
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    bool nodeopen = ImGui::TreeNodeEx("Global Assets", flags);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        m_editorContext.assetBrowserSelectedPath = "GlobalAssets";
    }
    if (nodeopen)
    {
        DrawAssetBrowserTreeNode("GlobalAssets", true);

        ImGui::TreePop();
    }

    flags = m_editorContext.assetBrowserTreeFlags;
    if (m_editorContext.assetBrowserSelectedPath == "Scenes")
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    nodeopen = ImGui::TreeNodeEx("Scenes", flags);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        m_editorContext.assetBrowserSelectedPath = "Scenes";
    }
    if (nodeopen)
    {
        std::vector<std::string> folders = vfs.GetFolders("Scenes");

        std::string scenePath = vfs.GetMountPoint("Scenes");
        // Filter out any folders that don't have a scene file
        for (int i = 0; i < folders.size(); i++)
        {
            std::string sceneFilePath = scenePath + "/" + folders[i] + "/" + folders[i] + ".scene";
            if (!PlanetoidEngine::FileIO::FileExists(sceneFilePath))
            {
                folders.erase(folders.begin() + i);
                i--;
            }
        }

        DrawAssetBrowserTreeNodesFromVector("Scenes", folders);

        ImGui::TreePop();
    }

    flags = m_editorContext.assetBrowserTreeFlags;
    if (m_editorContext.assetBrowserSelectedPath == "SceneAssets")
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    nodeopen = ImGui::TreeNodeEx("Scene Assets", flags);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        m_editorContext.assetBrowserSelectedPath = "SceneAssets";
    }
    if (nodeopen)
    {
        DrawAssetBrowserTreeNode("SceneAssets", true);

        ImGui::TreePop();
    }

    flags = m_editorContext.assetBrowserTreeFlags;
    if (m_editorContext.assetBrowserSelectedPath == "CodeProjectRoot/User")
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    nodeopen = ImGui::TreeNodeEx("Scripts");
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        m_editorContext.assetBrowserSelectedPath = "CodeProjectRoot/User";
    }
    if (nodeopen)
    {
        DrawAssetBrowserTreeNode("CodeProjectRoot/User", true);

        ImGui::TreePop();
    }
}

void Starship::DrawBrowserView()
{
    if (m_editorContext.assetBrowserSelectedPath == "")
    {
        return;
    }

    if (ImGui::Button("^"))
    {
        if (m_editorContext.assetBrowserSelectedPath != "GlobalAssets" && m_editorContext.assetBrowserSelectedPath != "Scenes" && m_editorContext.assetBrowserSelectedPath != "CodeProjectRoot/User")
        {
            if (m_editorContext.assetBrowserSelectedPath == "SceneAssets")
            {
                m_editorContext.assetBrowserSelectedPath = "Scenes/" + m_editorContext.projectContext.currentSceneName;
            }
            else
            {
                m_editorContext.assetBrowserSelectedPath = m_editorContext.assetBrowserSelectedPath.substr(0, m_editorContext.assetBrowserSelectedPath.find_last_of("/"));
            }
        }
    }

    static float padding = 16.0f;
    static float thumbnailSize = 128.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
    {
        columnCount = 1;
    }

    ImGui::Columns(columnCount, nullptr, false);
    for (int i = 0; i < columnCount; i++)
    {
        if (columnCount > 1)
        {
            ImGui::SetColumnWidth(i, cellSize);
        }
    }

    if (m_editorContext.playState == EditorContext::PlayState::Stopped)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
        {
            ImGui::OpenPopup("Asset Browser Context Menu");
        }

        if (ImGui::BeginPopup("Asset Browser Context Menu"))
        {
            if (ImGui::MenuItem("Create Folder"))
            {
                m_editorContext.showNewFolderPopup = true;
            }
            if (ImGui::MenuItem("Create Scene"))
            {
                m_editorContext.assetBrowserSelectedPath = "Scenes";
                m_editorContext.showNewScenePopup = true;
            }
            if (ImGui::MenuItem("Create Script"))
            {
                // Check if string starts with "CodeProjectRoot/User"
                if (m_editorContext.assetBrowserSelectedPath.find("CodeProjectRoot/User") != std::string::npos)
                {
                    m_editorContext.showNewScriptPopup = true;
                }
            }
            ImGui::EndPopup();
        }
    }

    std::vector<std::string> folders = vfs.GetFolders(m_editorContext.assetBrowserSelectedPath);
    std::vector<std::string> files = vfs.GetFiles(m_editorContext.assetBrowserSelectedPath);
    int currentIndex = 0;

    static std::string hoveredFolder = "";
    static int hoveredFolderIndex = -1;
    for (int i = 0; i < folders.size(); i++)
    {
        ImGui::ImageButton(reinterpret_cast<void*>(m_editorContext.folderTexture.GetHandle()), ImVec2(thumbnailSize, thumbnailSize));
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_editorContext.playState == EditorContext::PlayState::Stopped)
            {
                hoveredFolder = folders[i];
                hoveredFolderIndex = i;
                ImGui::OpenPopup("Folder Context Menu");
            }
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            m_editorContext.assetBrowserSelectedPath = m_editorContext.assetBrowserSelectedPath + "/" + folders[i];
        }
        ImGui::TextWrapped(folders[i].c_str());
        ImGui::NextColumn();
    }

    static std::string hoveredFile = "";
    static int hoveredFileIndex = -1;
    for (int i = 0; i < files.size(); i++)
    {
        unsigned int texHandle = m_editorContext.fileTexture.GetHandle();
        std::string physicalPath = vfs.ResolveVirtualPath(m_editorContext.assetBrowserSelectedPath + "/" + files[i]);
        // Check if the file is a png
        if (files[i].find(".png") != std::string::npos)
        {
            PlanetoidEngine::Asset* texAsset = assetStore.GetOrCreateAsset(physicalPath);
            if (texAsset)
            {
                if (texAsset->type == PlanetoidEngine::AssetType::Texture)
                {
                    texHandle = static_cast<PlanetoidEngine::Texture*>(texAsset->data)->GetHandle();
                }
            }
        }

        ImGui::ImageButton(reinterpret_cast<void*>(texHandle), ImVec2(thumbnailSize, thumbnailSize));

        if (files[i].find(".png") != std::string::npos && m_editorContext.playState == EditorContext::PlayState::Stopped)
        {
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("Texture", physicalPath.c_str(), physicalPath.size() + 1);
                ImGui::EndDragDropSource();
            }
        }
        else if (files[i].find(".scene") != std::string::npos && m_editorContext.playState == EditorContext::PlayState::Stopped)
        {
            if (ImGui::BeginDragDropSource())
            {
                std::string sceneName = files[i].substr(0, files[i].find_last_of("."));
                ImGui::SetDragDropPayload("Scene", sceneName.c_str(), sceneName.size() + 1);
                ImGui::EndDragDropSource();
            }
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_editorContext.playState == EditorContext::PlayState::Stopped)
            {
                hoveredFile = files[i];
                hoveredFileIndex = i;
                ImGui::OpenPopup("File Context Menu");
            }

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_editorContext.playState == EditorContext::PlayState::Stopped)
            {
                std::string filePath = m_editorContext.assetBrowserSelectedPath + "/" + files[i];
                if (filePath.find(".scene") != std::string::npos)
                {
                    // Strip the extension
                    std::string sceneName = files[i].substr(0, files[i].find_last_of("."));
                    if (LoadScene(sceneName))
                    {
                        m_editorContext.projectContext.currentSceneName = sceneName;
                    }
                }
                else if (filePath.find(".h") != std::string::npos || filePath.find(".cpp") != std::string::npos)
                {
                    std::string command = "code \"" + vfs.ResolveVirtualPath("CodeProjectRoot") + "\" \"" + vfs.ResolveVirtualPath(filePath) + "\"";
                    system(command.c_str());
                }
            }
        }
        

        ImGui::TextWrapped(files[i].c_str());
        ImGui::NextColumn();
    }

    if (ImGui::BeginPopup("Folder Context Menu"))
    {
        if (ImGui::MenuItem("Open"))
        {
            m_editorContext.assetBrowserSelectedPath = m_editorContext.assetBrowserSelectedPath + "/" + hoveredFolder;
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("File Context Menu"))
    {
        std::string filePath = m_editorContext.assetBrowserSelectedPath + "/" + hoveredFile;
        /**/
        if (filePath.find(".scene") != std::string::npos)
        {
            if (ImGui::MenuItem("Open"))
            {
                // Strip the extension
                std::string sceneName = hoveredFile.substr(0, hoveredFile.find_last_of("."));
                if (LoadScene(sceneName))
                {
                    m_editorContext.projectContext.currentSceneName = sceneName;
                }
            }
            if (ImGui::MenuItem("Set as Startup Scene"))
            {
                std::string sceneName = hoveredFile.substr(0, hoveredFile.find_last_of("."));
                m_editorContext.projectContext.startupSceneName = sceneName;
            }
        }
        else if (filePath.find(".h") != std::string::npos || filePath.find(".cpp") != std::string::npos)
        {
            if (ImGui::MenuItem("Open"))
            {
                std::string command = "code \"" + vfs.ResolveVirtualPath("CodeProjectRoot") + "\" \"" + vfs.ResolveVirtualPath(filePath) + "\"";
                system(command.c_str());
            }
        }
        ImGui::EndPopup();
    }
}

void Starship::DrawAssetBrowserTreeNode(const std::string& path, bool recursive)
{
    std::vector<std::string> folders = vfs.GetFolders(path);

    for (int i = 0; i < folders.size(); i++)
    {
        std::vector<std::string> childFolders = vfs.GetFolders(path + "/" + folders[i]);
        ImGuiTreeNodeFlags flags = m_editorContext.assetBrowserTreeFlags;
        std::string folderPath = path + "/" + folders[i];
        if (m_editorContext.assetBrowserSelectedPath == folderPath)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (childFolders.size() == 0 || !recursive)
        {
            bool nodeOpen = ImGui::TreeNodeEx(folders[i].c_str(), flags | ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_editorContext.assetBrowserSelectedPath = path + "/" + folders[i];
            }
            if (nodeOpen)
            {
                ImGui::TreePop();
            }
        }
        else
        {
            bool nodeOpen = ImGui::TreeNodeEx(folders[i].c_str(), flags);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_editorContext.assetBrowserSelectedPath = path + "/" + folders[i];
            }
            if (nodeOpen)
            {
                if (recursive)
                {
                    DrawAssetBrowserTreeNode(path + "/" + folders[i]);
                }
                ImGui::TreePop();
            }
        }
        
    }
}

void Starship::DrawAssetBrowserTreeNodesFromVector(const std::string& basePath, const std::vector<std::string>& paths)
{
    for (int i = 0; i < paths.size(); i++)
    {
        ImGuiTreeNodeFlags flags = m_editorContext.assetBrowserTreeFlags;
        if (m_editorContext.assetBrowserSelectedPath == basePath + "/" + paths[i])
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        bool nodeOpen = ImGui::TreeNodeEx(paths[i].c_str(), flags | ImGuiTreeNodeFlags_Leaf);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_editorContext.assetBrowserSelectedPath = basePath + "/" + paths[i];
        }
        if (nodeOpen)
        {
            ImGui::TreePop();
        }
    }
}

void Starship::GenerateCodeProject()
{
    if (m_editorContext.projectContext.rootPath == "")
    {
        PlanetoidEngine::Log::Error("No project loaded");
        return;
    }
    
    if (!vfs.IsMounted("ProjectRoot"))
    {
        PlanetoidEngine::Log::Error("Project root is not mounted");
        return;
    }

    std::string CodeProjectRoot = vfs.ResolveVirtualPath("ProjectRoot/ScriptEngine");
    PlanetoidEngine::FileIO::FolderCreate(CodeProjectRoot);
    vfs.Mount("CodeProjectRoot", CodeProjectRoot);

    PlanetoidEngine::FileIO::FolderCreate(CodeProjectRoot + "/build");
    PlanetoidEngine::FileIO::FolderCreate(CodeProjectRoot + "/Generated");
    PlanetoidEngine::FileIO::FolderCreate(CodeProjectRoot + "/User");
    PlanetoidEngine::FileIO::FileCreate(CodeProjectRoot + "/CMakeLists.txt");

    PlanetoidEngine::File cmakelist(CodeProjectRoot + "/CMakeLists.txt");
    if (!PlanetoidEngine::FileIO::FileExists(cmakelist.GetPath()))
    {
        PlanetoidEngine::Log::Error("Failed to create CMakeLists.txt");
        return;
    }
    GenerateCodeProjectCMakeLists(cmakelist);

    PlanetoidEngine::FileIO::FileCreate(CodeProjectRoot + "/Generated/main.cpp");

    PlanetoidEngine::File main(CodeProjectRoot + "/Generated/main.cpp");
    if (!PlanetoidEngine::FileIO::FileExists(main.GetPath()))
    {
        PlanetoidEngine::Log::Error("Failed to create main.cpp");
        return;
    }
    GenerateCodeProjectMain(main);

    PlanetoidEngine::FileIO::FileCreate(CodeProjectRoot + "/Generated/ScriptCore.h");

    PlanetoidEngine::File scriptCore(CodeProjectRoot + "/Generated/ScriptCore.h");
    if (!PlanetoidEngine::FileIO::FileExists(CodeProjectRoot + "/Generated/ScriptCore.h"))
    {
        PlanetoidEngine::Log::Error("Failed to create ScriptCore.h");
        return;
    }
    GenerateCodeProjectScriptCore(scriptCore);

    PlanetoidEngine::FileIO::FileCreate(CodeProjectRoot + "/User/RegisterScripts.cpp");

    PlanetoidEngine::File registerScripts(CodeProjectRoot + "/User/RegisterScripts.cpp");
    if (!PlanetoidEngine::FileIO::FileExists(CodeProjectRoot + "/User/RegisterScripts.cpp"))
    {
        PlanetoidEngine::Log::Error("Failed to create RegisterScripts.cpp");
        return;
    }
    GenerateCodeProjectRegisterScripts(registerScripts);

    PlanetoidEngine::FileIO::FileCreate(CodeProjectRoot + "/Generated/build.bat");

    PlanetoidEngine::File buildbat(CodeProjectRoot + "/Generated/build.bat");
    if (!PlanetoidEngine::FileIO::FileExists(buildbat.GetPath()))
    {
        PlanetoidEngine::Log::Error("Failed to create build.bat");
        return;
    }
    GenerateCodeProjectBuildBat(buildbat);

    /*
    - ScriptEngine
    - - CMakeLists.txt
    - - build/
    - - Generated/
    - - - main.cpp
    - - User/
    - - - Script Files go here
    */

    std::string command = "@echo off && cd \"" + vfs.ResolveVirtualPath("CodeProjectRoot/Generated") + "\" && build.bat"; 
    int res = system(command.c_str());

    if (TryLoadScriptDLL())
    {
        PlanetoidEngine::Log::Success("Code project generated");
    }
    else
    {
        PlanetoidEngine::Log::Error("Failed to build generated code project!");
    }
}

void Starship::GenerateCodeProjectCMakeLists(PlanetoidEngine::File& file)
{
    file.AppendTextContents("# Generated by Starship Editor " + m_editorContext.GetVersionString() + "\n");
    file.AppendTextContents("cmake_minimum_required(VERSION 3.25.0)\n");
    
    std::string ProjectName = "PlanetoidEngine-" + m_editorContext.projectContext.name + "-ScriptEngine";
    file.AppendTextContents("project(" + ProjectName + ")\n\n");

    std::string DLLPath = vfs.ResolveVirtualPath("EngineRoot");
    file.AppendTextContents("set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \"../bin\")\n\n");

    std::string EngineIncludePath = vfs.ResolveVirtualPath("EngineRoot/../Core/include");
    std::string UserFolderPath = vfs.ResolveVirtualPath("CodeProjectRoot/User");
    std::string GeneratedFolderPath = vfs.ResolveVirtualPath("CodeProjectRoot/Generated");
    file.AppendTextContents("include_directories(\"" + EngineIncludePath + "\" \"" + UserFolderPath + "\" \"" + GeneratedFolderPath + "\")\n\n");

    file.AppendTextContents("file(GLOB_RECURSE USER_SOURCES \"User/*.cpp\")\n");
    file.AppendTextContents("add_library(" + ProjectName + " SHARED Generated/main.cpp ${USER_SOURCES})\n");

    std::string EngineDLLPath = vfs.ResolveVirtualPath("EngineRoot");
    file.AppendTextContents("target_link_libraries(" + ProjectName + " \"" + EngineDLLPath + "/libPlanetoidEngine-Core.dll\")\n\n");

    file.AppendTextContents("add_compile_definitions(PE_WINDOWS)");
}

void Starship::GenerateCodeProjectMain(PlanetoidEngine::File& file)
{
    file.AppendTextContents("#include <Planetoid/PlanetoidCore.h>\n\n");
    file.AppendTextContents("#include \"ScriptCore.h\"\n\n");
    file.AppendTextContents("PlanetoidEngine::ScriptDictionary g_scriptDictionary;\n\n");
    file.AppendTextContents("void OnAttach()\n");
    file.AppendTextContents("{\n");
    file.AppendTextContents("    RegisterScripts();\n");
    file.AppendTextContents("}\n\n");
    file.AppendTextContents("void OnDetach()\n");
    file.AppendTextContents("{\n\n");
    file.AppendTextContents("}\n\n");
    file.AppendTextContents("PlanetoidEngine::IScript* GetScript(const std::string& name)\n");
    file.AppendTextContents("{\n");
    file.AppendTextContents("    return g_scriptDictionary.GetScript(name);\n");
    file.AppendTextContents("}\n\n");
    file.AppendTextContents("std::vector<std::string> GetRegisteredScriptNames()\n");
    file.AppendTextContents("{\n");
    file.AppendTextContents("    return g_scriptDictionary.GetScriptNames();\n");
    file.AppendTextContents("}");
}

void Starship::GenerateCodeProjectRegisterScripts(PlanetoidEngine::File& file)
{
    file.AppendTextContents("#include <ScriptCore.h>\n\n");
    file.AppendTextContents("void RegisterScripts()\n");
    file.AppendTextContents("{\n");
    file.AppendTextContents("    // Call REGISTER_SCRIPT for each script\n");
    file.AppendTextContents("}\n");
}

void Starship::GenerateCodeProjectScriptCore(PlanetoidEngine::File& file)
{
    file.AppendTextContents("#pragma once\n\n");
    file.AppendTextContents("#include <Planetoid/ScriptEngine/ScriptAPI.h>\n");
    file.AppendTextContents("#include <Planetoid/ScriptEngine/ScriptDictionary.h>\n\n");
    file.AppendTextContents("extern PlanetoidEngine::ScriptDictionary g_scriptDictionary;\n");
    file.AppendTextContents("#define REGISTER_SCRIPT(name, type) g_scriptDictionary.AddScript<type>(name)\n\n");
    file.AppendTextContents("extern \"C\"\n");
    file.AppendTextContents("{\n");
    file.AppendTextContents("    void OnAttach();\n");
    file.AppendTextContents("    void OnDetach();\n");
    file.AppendTextContents("    PlanetoidEngine::IScript* GetScript(const std::string& name);\n");
    file.AppendTextContents("    std::vector<std::string> GetRegisteredScriptNames();");
    file.AppendTextContents("}\n\n");
    file.AppendTextContents("void RegisterScripts();\n");
}

void Starship::GenerateCodeProjectBuildBat(PlanetoidEngine::File& file)
{
    file.AppendTextContents("@echo off\n");
    file.AppendTextContents("cd ../build\n");
    file.AppendTextContents("cmake ..\n");
    file.AppendTextContents("cmake --build .\n");
    file.AppendTextContents("cd ../Generated\n");
    file.AppendTextContents("exit /b 1");
}