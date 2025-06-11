#include <Planetoid/Core/InputManager.h>

#include <GLFW/glfw3.h>

#include <Planetoid/Core/Application.h>

namespace PlanetoidEngine
{
    ScrollWheelDelegate InputManager::OnScrollWheel;
    FileDropCallback InputManager::OnFileDrop;

    std::unordered_map<std::string, Input> InputManager::m_inputs;
    std::unordered_map<std::string, float> InputManager::m_inputTimestamps;
    
    void InputManager::Init()
    {
        glfwSetScrollCallback(glfwGetCurrentContext(), [](GLFWwindow* window, double xoffset, double yoffset)
        {
            InputManager::OnScrollWheel.Broadcast((float)xoffset, (float)yoffset);
        });

        glfwSetDropCallback(glfwGetCurrentContext(), [](GLFWwindow* window, int count, const char** paths)
        {
            InputManager::OnFileDrop.Broadcast(count, paths);
        });
    }

    void InputManager::Poll(float deltaTime)
    {
        for (auto& input : m_inputs)
        {
            Input& i = input.second;
            const std::string& name = input.first;
            bool pressed = false;
            if (i.type == Input::InputType::Keyboard)
            {
                pressed = IsKeyPressed(i.key);
            }
            else if (i.type == Input::InputType::Mouse)
            {
                pressed = IsMouseButtonPressed(i.key);
            }
            else if (i.type == Input::InputType::Gamepad)
            {
                pressed = IsGamepadButtonPressed(i.gamepad, i.key);
            }

            if (pressed)
            {
                if (m_inputTimestamps.find(name) == m_inputTimestamps.end())
                {
                    m_inputTimestamps[name] = 0.0f;
                    if (i.action == Input::InputAction::Press || i.action == Input::InputAction::Hold)
                    {
                        i.OnActivation.Broadcast();
                    }
                }
                else
                {
                    m_inputTimestamps[name] += deltaTime;
                    if (i.action == Input::InputAction::Hold && m_inputTimestamps[name] >= i.threshold)
                    {
                        i.OnActivation.Broadcast();
                    }
                }
            }
            else
            {
                if (m_inputTimestamps.find(name) != m_inputTimestamps.end())
                {
                    if (i.action == Input::InputAction::Release)
                    {
                        i.OnActivation.Broadcast();
                    }
                    else if (i.action == Input::InputAction::Tap && m_inputTimestamps[name] < i.threshold)
                    {
                        i.OnActivation.Broadcast();
                    }
                    m_inputTimestamps.erase(name);
                }
            
            }
        }
    }

    bool InputManager::IsKeyPressed(int key)
    {
        GLFWwindow* window = glfwGetCurrentContext();
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool InputManager::IsMouseButtonPressed(int button)
    {
        GLFWwindow* window = glfwGetCurrentContext();
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    float InputManager::GetMouseX()
    {
        GLFWwindow* window = glfwGetCurrentContext();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return (float)x;
    }

    float InputManager::GetMouseY()
    {
        GLFWwindow* window = glfwGetCurrentContext();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return (float)y;
    }

    void InputManager::GetMousePosition(float& x, float& y)
    {
        GLFWwindow* window = glfwGetCurrentContext();
        double _x, _y;
        glfwGetCursorPos(window, &_x, &_y);
        x = (float)_x;
        y = (float)_y;
    }

    bool InputManager::IsGamepadConnected(int gamepad)
    {
        return glfwJoystickPresent(gamepad);
    }

    bool InputManager::IsGamepadButtonPressed(int gamepad, int button)
    {
        int count;
        const unsigned char* buttons = glfwGetJoystickButtons(gamepad, &count);
        return buttons[button] == GLFW_PRESS;
    }

    float InputManager::GetGamepadAxis(int gamepad, int axis)
    {
        int count;
        const float* axes = glfwGetJoystickAxes(gamepad, &count);
        return axes[axis];
    }

    void InputManager::RegisterInput(const std::string& name, Input input)
    {
        m_inputs[name] = input;
    }

    void InputManager::UnregisterInput(const std::string& name)
    {
        m_inputs.erase(name);
    }

    Input& InputManager::GetInput(const std::string& name)
    {
        return m_inputs[name];
    }

    void InputManager::ClearInputs()
    {
        m_inputs.clear();
    }
}