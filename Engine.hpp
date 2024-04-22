//
//  Engine.hpp
//  game_engine
//
//  Created by Nicholas Way on 1/25/24.
//

#ifndef Engine_hpp
#define Engine_hpp

#ifdef _WIN32
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <optional>

//#include "MapHelper.h"
#include "rapidjson/document.h"
#include "Actor.hpp"
#include "EngineUtils.h"
#include "Renderer.hpp"
#include "ImageLoader.h"
#include "AudioLoader.h"
#include "InputManager.hpp"
#include "ComponentManager.hpp"
#include "EventBus.hpp"


struct LocationData{
    std::vector<Actor*> actors_present;
    int collider_nums = 0;
};


class Engine
{
public:
    void StartGameLoop();
    
    // scene functions (may be moved to a scene manager)
    static luabridge::LuaRef FindActorByName(const std::string& name);
    static luabridge::LuaRef FindAllActorsByName(const std::string& name);

    static void AddActor(std::shared_ptr<Actor> actor);
    static luabridge::LuaRef InstantiateActor(const std::string& name);
    static void ProcessAddActors();
    static void RemoveActor(luabridge::LuaRef actor);
    static void ProcessRemoveActors();
    static void ClearActors();
    static void AddDontDestroyOnLoadActors();

    static void LoadScene(const std::string& name);
    static std::string GetCurrentScene();
    static void DontDestroyOnLoad(luabridge::LuaRef actor);

private:
    void init();
    void CollectInput(SDL_Event& next_event);
    void Update();
    void Render();
    void Shutdown();

    Renderer renderer;

    /* Lua */
    lua_State* Lua_State = nullptr;

    /* Game Json */
    rapidjson::Document game_config;
    uint8_t clear_color[3] = { 255, 255, 255 };

    /* File IO */
    std::ifstream file;

    /* Game State */
    static inline std::vector<std::shared_ptr<Actor>> scene_actors;
    static inline std::vector<std::shared_ptr<Actor>> actors_to_add;
    static inline std::vector<std::shared_ptr<Actor>> actors_to_remove;
    static inline std::vector<std::shared_ptr<Actor>> actors_to_start;
    static inline std::vector<std::shared_ptr<Actor>> dont_destroy_actors;
    glm::vec2 cam_position = glm::vec2(0, 0);
    int cam_x_resolution = 640;
    int cam_y_resolution = 360;
    float zoom_factor = 1.0f;
	bool running = true;
    int quitting = 0;
    static inline std::string current_scene;
    static inline std::string loading_scene = "";
};

#endif /* Engine_hpp */
