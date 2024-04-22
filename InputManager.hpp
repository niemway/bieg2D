//
//  InputManager.hpp
//  game_engine
//
//  Created by Nicholas Way on 2/15/24.
//

#ifndef InputManager_hpp
#define InputManager_hpp

#include <stdio.h>

#include <SDL.h>
#include <unordered_map>
#include <vector>

#include "keycode_to_scancode.h"

#include "glm/glm.hpp"

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input
{
public:
    static void Init(); // Call before main loop begins.
    static void ProcessEvent(const SDL_Event & e); // Call every frame at start of event loop.
    static void LateUpdate();

    static bool GetKey(SDL_Scancode keycode);
    static bool GetKey_S(std::string key);
    static bool GetKeyDown(SDL_Scancode keycode);
    static bool GetKeyDown_S(std::string key);
    static bool GetKeyUp(SDL_Scancode keycode);
    static bool GetKeyUp_S(std::string key);

    static glm::vec2 GetMousePosition();

    static bool GetMouseButton(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    static float GetMouseWheelDelta();

    static int ConvertMouseButtonToIndex(const std::string& button_name);

private:
    static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
    static inline std::vector<SDL_Scancode> just_became_down_scancodes;
    static inline std::vector<SDL_Scancode> just_became_up_scancodes;
    
    static inline glm::vec2 mouse_position;
    static inline std::unordered_map<int, INPUT_STATE> mouse_states;
    static inline std::vector<int> just_became_down_mouse_buttons;
    static inline std::vector<int> just_became_up_mouse_buttons;

    static inline float mouse_scroll_this_frame = 0.0f;
};

#endif /* InputManager_hpp */
