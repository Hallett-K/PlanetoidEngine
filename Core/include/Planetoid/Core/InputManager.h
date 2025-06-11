#pragma once

#include <Planetoid/PlanetoidCore.h>
#include <Planetoid/Core/Delegate.h>

namespace PlanetoidEngine
{
    DECLARE_DELEGATE(InputActivationDelegate);

    struct PE_API Input 
    {
        enum class InputAction
        {
            Press,
            Hold,
            Release,
            Tap
        };

        enum class InputType
        {
            Keyboard,
            Mouse,
            Gamepad
        };

        InputType type = InputType::Keyboard;
        int key = 0;
        int gamepad = 0; // Only used for gamepad inputs
        InputAction action = InputAction::Press;
        float threshold = 0.0f; // For holds and taps
        int tapCount = 1; // For taps

        InputActivationDelegate OnActivation;

        Input() = default;
        Input(InputType type, int key, InputAction action, float threshold = 0.0f)
            : type(type), key(key), action(action), threshold(threshold)
        {}
    };

    DECLARE_DELEGATE(ScrollWheelDelegate, float, float);
    DECLARE_DELEGATE(FileDropCallback, int, const char**);

    class PE_API InputManager
    {
    public:
        static void Init();
        static void Poll(float deltaTime);

        // Keyboard and Mouse
        static bool IsKeyPressed(int key);
        static bool IsMouseButtonPressed(int button);
        static float GetMouseX();
        static float GetMouseY();
        static void GetMousePosition(float& x, float& y);

        static ScrollWheelDelegate OnScrollWheel;
        static FileDropCallback OnFileDrop;

        // Gamepad
        static bool IsGamepadConnected(int gamepad);
        static bool IsGamepadButtonPressed(int gamepad, int button);
        static float GetGamepadAxis(int gamepad, int axis);

        // Enhanced Input
        static void RegisterInput(const std::string& name, Input input);
        static void UnregisterInput(const std::string& name);
        static Input& GetInput(const std::string& name);
        static void ClearInputs();

    private:
        static std::unordered_map<std::string, Input> m_inputs;
        static std::unordered_map<std::string, float> m_inputTimestamps;
        static std::unordered_map<std::string, int> m_inputTapCounts;
    };
}

// From GLFW/glfw3.h

/* Printable keys */
#define PE_KEY_SPACE              32
#define PE_KEY_APOSTROPHE         39
#define PE_KEY_COMMA              44 
#define PE_KEY_MINUS              45 
#define PE_KEY_PERIOD             46 
#define PE_KEY_SLASH              47 
#define PE_KEY_0                  48
#define PE_KEY_1                  49
#define PE_KEY_2                  50
#define PE_KEY_3                  51
#define PE_KEY_4                  52
#define PE_KEY_5                  53
#define PE_KEY_6                  54
#define PE_KEY_7                  55
#define PE_KEY_8                  56
#define PE_KEY_9                  57
#define PE_KEY_SEMICOLON          59 
#define PE_KEY_EQUAL              61
#define PE_KEY_A                  65
#define PE_KEY_B                  66
#define PE_KEY_C                  67
#define PE_KEY_D                  68
#define PE_KEY_E                  69
#define PE_KEY_F                  70
#define PE_KEY_G                  71
#define PE_KEY_H                  72
#define PE_KEY_I                  73
#define PE_KEY_J                  74
#define PE_KEY_K                  75
#define PE_KEY_L                  76
#define PE_KEY_M                  77
#define PE_KEY_N                  78
#define PE_KEY_O                  79
#define PE_KEY_P                  80
#define PE_KEY_Q                  81
#define PE_KEY_R                  82
#define PE_KEY_S                  83
#define PE_KEY_T                  84
#define PE_KEY_U                  85
#define PE_KEY_V                  86
#define PE_KEY_W                  87
#define PE_KEY_X                  88
#define PE_KEY_Y                  89
#define PE_KEY_Z                  90
#define PE_KEY_LEFT_BRACKET       91
#define PE_KEY_BACKSLASH          92
#define PE_KEY_RIGHT_BRACKET      93
#define PE_KEY_GRAVE_ACCENT       96

/* Function keys */
#define PE_KEY_ESCAPE             256
#define PE_KEY_ENTER              257
#define PE_KEY_TAB                258
#define PE_KEY_BACKSPACE          259
#define PE_KEY_INSERT             260
#define PE_KEY_DELETE             261
#define PE_KEY_RIGHT              262
#define PE_KEY_LEFT               263
#define PE_KEY_DOWN               264
#define PE_KEY_UP                 265
#define PE_KEY_PAGE_UP            266
#define PE_KEY_PAGE_DOWN          267
#define PE_KEY_HOME               268
#define PE_KEY_END                269
#define PE_KEY_CAPS_LOCK          280
#define PE_KEY_SCROLL_LOCK        281
#define PE_KEY_NUM_LOCK           282
#define PE_KEY_PRINT_SCREEN       283
#define PE_KEY_PAUSE              284
#define PE_KEY_F1                 290
#define PE_KEY_F2                 291
#define PE_KEY_F3                 292
#define PE_KEY_F4                 293
#define PE_KEY_F5                 294
#define PE_KEY_F6                 295
#define PE_KEY_F7                 296
#define PE_KEY_F8                 297
#define PE_KEY_F9                 298
#define PE_KEY_F10                299
#define PE_KEY_F11                300
#define PE_KEY_F12                301
#define PE_KEY_KEYPAD_0           320
#define PE_KEY_KEYPAD_1           321
#define PE_KEY_KEYPAD_2           322
#define PE_KEY_KEYPAD_3           323
#define PE_KEY_KEYPAD_4           324
#define PE_KEY_KEYPAD_5           325
#define PE_KEY_KEYPAD_6           326
#define PE_KEY_KEYPAD_7           327
#define PE_KEY_KEYPAD_8           328
#define PE_KEY_KEYPAD_9           329
#define PE_KEY_KEYPAD_DECIMAL     330
#define PE_KEY_KEYPAD_DIVIDE      331
#define PE_KEY_KEYPAD_MULTIPLY    332
#define PE_KEY_KEYPAD_SUBTRACT    333
#define PE_KEY_KEYPAD_ADD         334
#define PE_KEY_KEYPAD_ENTER       335
#define PE_KEY_KEYPAD_EQUAL       336
#define PE_KEY_LEFT_SHIFT         340
#define PE_KEY_LEFT_CONTROL       341
#define PE_KEY_LEFT_ALT           342
#define PE_KEY_LEFT_SUPER         343
#define PE_KEY_RIGHT_SHIFT        344
#define PE_KEY_RIGHT_CONTROL      345
#define PE_KEY_RIGHT_ALT          346
#define PE_KEY_RIGHT_SUPER        347
#define PE_KEY_MENU               348

#define PE_MOUSE_BUTTON_1         0
#define PE_MOUSE_BUTTON_2         1
#define PE_MOUSE_BUTTON_3         2
#define PE_MOUSE_BUTTON_4         3
#define PE_MOUSE_BUTTON_5         4
#define PE_MOUSE_BUTTON_6         5
#define PE_MOUSE_BUTTON_7         6
#define PE_MOUSE_BUTTON_8         7
#define PE_MOUSE_BUTTON_LEFT      PE_MOUSE_BUTTON_1
#define PE_MOUSE_BUTTON_RIGHT     PE_MOUSE_BUTTON_2
#define PE_MOUSE_BUTTON_MIDDLE    PE_MOUSE_BUTTON_3

#define PE_GAMEPAD_NUM_1             0
#define PE_GAMEPAD_NUM_2             1
#define PE_GAMEPAD_NUM_3             2
#define PE_GAMEPAD_NUM_4             3
#define PE_GAMEPAD_NUM_5             4
#define PE_GAMEPAD_NUM_6             5
#define PE_GAMEPAD_NUM_7             6
#define PE_GAMEPAD_NUM_8             7
#define PE_GAMEPAD_NUM_9             8
#define PE_GAMEPAD_NUM_10            9
#define PE_GAMEPAD_NUM_11            10
#define PE_GAMEPAD_NUM_12            11
#define PE_GAMEPAD_NUM_13            12
#define PE_GAMEPAD_NUM_14            13
#define PE_GAMEPAD_NUM_15            14
#define PE_GAMEPAD_NUM_16            15

#define PE_GAMEPAD_BUTTON_A               0
#define PE_GAMEPAD_BUTTON_B               1
#define PE_GAMEPAD_BUTTON_X               2
#define PE_GAMEPAD_BUTTON_Y               3
#define PE_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define PE_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define PE_GAMEPAD_BUTTON_BACK            6
#define PE_GAMEPAD_BUTTON_START           7
#define PE_GAMEPAD_BUTTON_GUIDE           8
#define PE_GAMEPAD_BUTTON_LEFT_THUMB      9
#define PE_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define PE_GAMEPAD_BUTTON_DPAD_UP         11
#define PE_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define PE_GAMEPAD_BUTTON_DPAD_DOWN       13
#define PE_GAMEPAD_BUTTON_DPAD_LEFT       14

#define PE_GAMEPAD_BUTTON_CROSS       PE_GAMEPAD_BUTTON_A
#define PE_GAMEPAD_BUTTON_CIRCLE      PE_GAMEPAD_BUTTON_B
#define PE_GAMEPAD_BUTTON_SQUARE      PE_GAMEPAD_BUTTON_X
#define PE_GAMEPAD_BUTTON_TRIANGLE    PE_GAMEPAD_BUTTON_Y

#define PE_GAMEPAD_AXIS_LEFT_X        0
#define PE_GAMEPAD_AXIS_LEFT_Y        1
#define PE_GAMEPAD_AXIS_RIGHT_X       2
#define PE_GAMEPAD_AXIS_RIGHT_Y       3
#define PE_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define PE_GAMEPAD_AXIS_RIGHT_TRIGGER 5