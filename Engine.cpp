//
//  Engine.cpp
//  game_engine
//
//  Created by Nicholas Way on 1/25/24.
//

#ifdef _WIN32
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include "Engine.hpp"
// #include "MapHelper.h"
#include "glm/glm.hpp"
#include <iostream>
#include <sstream>

#include <SDL.h>

#include "EngineUtils.h"
#include "SceneLoader.hpp"
#include "ActorTemplateManager.hpp"
#include "Helper.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Rigidbody.hpp"

void Engine::init()
{
    // check if resources directory exists
    if(!EngineUtils::DirectoryExists("resources")){
        std::cout << "error: resources/ missing";
        exit(0);
    }

    // check if the file resources/game.config exists
    if(!EngineUtils::DirectoryExists("resources/game.config")){
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
    
    EngineUtils::ReadJsonFile("resources/game.config", game_config);

    // get info from rendering.config
    if(EngineUtils::DirectoryExists("resources/rendering.config")){
        rapidjson::Document rendering_config;
        EngineUtils::ReadJsonFile("resources/rendering.config", rendering_config);

        if(rendering_config.HasMember("x_resolution")){
            cam_x_resolution = rendering_config["x_resolution"].GetInt();
        }
        if(rendering_config.HasMember("y_resolution")){
            cam_y_resolution = rendering_config["y_resolution"].GetInt();
        }
        // read clear color
        if(rendering_config.HasMember("clear_color_r")){
            clear_color[0] = rendering_config["clear_color_r"].GetInt();
        }
        if(rendering_config.HasMember("clear_color_g")){
            clear_color[1] = rendering_config["clear_color_g"].GetInt();
        }
        if(rendering_config.HasMember("clear_color_b")){
            clear_color[2] = rendering_config["clear_color_b"].GetInt();
        }
        // initialize render scale
        if(rendering_config.HasMember("zoom_factor")){
            zoom_factor = rendering_config["zoom_factor"].GetDouble();
        }
    }

    std::string game_title = "";
    if(game_config.HasMember("game_title")){
        game_title = game_config["game_title"].GetString();
    }

    // initialize the renderer
    renderer.init(game_title, cam_y_resolution, cam_x_resolution, clear_color, zoom_factor);
    ImageLoader::GetInstance().Init(renderer.renderer);
    
    // initialize audio driver
    AudioLoader::GetInstance().Init();
    
    ComponentManager::Initialize();

    if(game_config.HasMember("initial_scene")){
        std::string initial_scene = game_config["initial_scene"].GetString();
        SceneLoader::LoadScene(initial_scene, this);
        current_scene = initial_scene;
    }
    else {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
    
    // call actor start functions
    ProcessAddActors();
    
    // initialize input
    Input::Init();
}

void Engine::StartGameLoop()
{
    //    file.open("debug_input.txt");

    init();

    while (running) {
        SDL_Event next_event;
        // collect input
        CollectInput(next_event);

        // clear previous frame
        renderer.ClearScreen();
        
        if(loading_scene != ""){
            // load scene
            ClearActors();
            AddDontDestroyOnLoadActors();
            SceneLoader::LoadScene(loading_scene, this);
            current_scene = loading_scene;
            loading_scene = "";
            ProcessAddActors();
        }

        // update game state
        Update();

        EventBus::ProcessPending();
        
        // advance physics
        if(Rigidbody::world_initialized)
            Rigidbody::world->Step(1.0f / 60.0f, 8, 3);

        Render();
        
        // end of frame updates
        Input::LateUpdate();
        
        Helper::SDL_RenderPresent498(renderer.renderer); 
    }

//    file.close();

    //Shutdown();
}

void Engine::CollectInput(SDL_Event& next_event)
{
    // get the next event
    while (Helper::SDL_PollEvent498(&next_event)) {
        if (next_event.type == SDL_QUIT) {
            running = false;
        }
        Input::ProcessEvent(next_event);
    }
}

void Engine::Update()
{
    // call Start() on every actor
    /*
    for(auto& actor : actors_to_start){
        actor->Start();
    }
    actors_to_start.clear();
    */
    for(auto& actor : scene_actors){
        actor->Start();
    }
    // call Update() on every actor
    for(auto& actor : scene_actors){
        actor->Update();
    }
    for(auto& actor : scene_actors){
        actor->LateUpdate();
    }
    ProcessAddActors();
    ProcessRemoveActors();
}

void Engine::Render()
{
    SDL_RenderSetScale(renderer.renderer, zoom_factor, zoom_factor);

    // render scene-space images
    renderer.RenderAllImages();

    // render UI images
    renderer.RenderAllUI();

    // render text
    renderer.RenderAllText();

    // render pixels
    renderer.RenderAllPixelCalls();
}

luabridge::LuaRef Engine::FindActorByName(const std::string& name)
{
    for(auto& actor : scene_actors){
        if(actor->name == name && !actor->marked_for_deletion){
            return luabridge::LuaRef(ComponentManager::GetLuaState(), actor.get());
        }
    }
    // also check the actors to add
    for(auto& actor : actors_to_add){
        if(actor->name == name){
            return luabridge::LuaRef(ComponentManager::GetLuaState(), actor.get());
        }
    }
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

luabridge::LuaRef Engine::FindAllActorsByName(const std::string& name)
{
    luabridge::LuaRef actor_table = ComponentManager::CreateEmptyTable();
    int actors_found = 0;
    for(auto& actor : scene_actors){
        if(actor->name == name && !actor->marked_for_deletion){
            actor_table[++actors_found] = actor.get();
        }
    }
    // also check the actors to add
    for(auto& actor : actors_to_add){
        if(actor->name == name){
            actor_table[++actors_found] = actor.get();
        }
    }
    if(actors_found > 0){
        return actor_table;
    }
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

void Engine::AddActor(std::shared_ptr<Actor> actor)
{
    actor.get()->InjectAllConvenienceReferences();
    actors_to_add.push_back(actor);
}

luabridge::LuaRef Engine::InstantiateActor(const std::string& template_name)
{
    Actor template_actor = ActorTemplateManager::GetActorTemplate(template_name);
    template_actor.actor_uid = SceneLoader::next_actor_uid++;
    std::shared_ptr<Actor> new_actor = std::make_shared<Actor>(template_actor);
    AddActor(new_actor);
    return luabridge::LuaRef(ComponentManager::GetLuaState(), new_actor.get());
}

void Engine::ProcessAddActors()
{
    // add actors
    for(auto& actor : actors_to_add){
        scene_actors.push_back(actor);
        actors_to_start.push_back(actor);
    }
    actors_to_add.clear();
}

void Engine::RemoveActor(luabridge::LuaRef actor)
{
    int actor_uid = actor.cast<Actor*>()->actor_uid;
    
    for(auto& a : scene_actors){
        if(a->actor_uid == actor_uid){
            a->DisableAllComponents();
            a->marked_for_deletion = true;
            actors_to_remove.push_back(a);
            return;
        }
    }
    // also check actors to add
    for(auto& a : actors_to_add){
        if(a->actor_uid == actor_uid){
            a->DisableAllComponents();
            a->marked_for_deletion = true;
            actors_to_remove.push_back(a);
            // actors_to_add.erase(std::remove(actors_to_add.begin(), actors_to_add.end(), a), actors_to_add.end());
            return;
        }
    }
}

void Engine::ProcessRemoveActors()
{
    // remove actors
    for(auto& actor : actors_to_remove){
        // check if the actor is in the scene
        if(std::find(scene_actors.begin(), scene_actors.end(), actor) != scene_actors.end()){
            actor->Destroy();
            scene_actors.erase(std::remove(scene_actors.begin(), scene_actors.end(), actor), scene_actors.end());
        }
        else if(std::find(actors_to_add.begin(), actors_to_add.end(), actor) != actors_to_add.end()){
            actor->Destroy();
            actors_to_add.erase(std::remove(actors_to_add.begin(), actors_to_add.end(), actor), actors_to_add.end());
        }
        else {
            std::cout << "error: removing actor not found in scene or actors to add";
        }

        // scene_actors.erase(std::remove(scene_actors.begin(), scene_actors.end(), actor), scene_actors.end());
    }
    actors_to_remove.clear();
}

void Engine::ClearActors()
{
    actors_to_start.clear();
    scene_actors.clear();
    actors_to_remove.clear();
    actors_to_add.clear();
}

void Engine::AddDontDestroyOnLoadActors()
{
    scene_actors.reserve(dont_destroy_actors.size());
    for(auto& actor : dont_destroy_actors){
        scene_actors.push_back(actor);
    }
}

void Engine::LoadScene(const std::string& name)
{
    loading_scene = name;
}

std::string Engine::GetCurrentScene()
{
    return current_scene;
}

void Engine::DontDestroyOnLoad(luabridge::LuaRef actor)
{
    int actor_uid = actor.cast<Actor*>()->actor_uid;
    for(auto& a : scene_actors){
        if(a->actor_uid == actor_uid){
            dont_destroy_actors.push_back(a);
            return;
        }
    }
    // also check actors to add
    for(auto& a : actors_to_add){
        if(a->actor_uid == actor_uid){
            dont_destroy_actors.push_back(a);
            return;
        }
    }
}
