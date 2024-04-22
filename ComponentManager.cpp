//
//  ComponentManager.cpp
//  game_engine
//
//  Created by Nicholas Way on 3/7/24.
//

#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "Actor.hpp"
#include "Helper.h"
#include "TextLoader.h"
#include "Rigidbody.hpp"
#include "Collision.hpp"
#include "Physics.hpp"
#include "EventBus.hpp"
#include "Animator.hpp"
#include "Animation.hpp"

void ComponentManager::Initialize() {
	InitializeState();
	InitializeFunctions();
	InitializeComponents();
}

void ComponentManager::InitializeState() {
	// create a new lua state
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
}

void ComponentManager::InitializeFunctions() {
	// register the function to load a component from a lua table
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", ComponentManager::Print)
		.addFunction("LogError", ComponentManager::PrintError)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass();
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Actor")
        .addFunction("Find", &Engine::FindActorByName)
        .addFunction("FindAll", &Engine::FindAllActorsByName)
		.addFunction("Instantiate", &Engine::InstantiateActor)
		.addFunction("Destroy", &Engine::RemoveActor)
        .endNamespace();
	// register the application namespace functions
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", ComponentManager::Application_Quit)
		.addFunction("Sleep", ComponentManager::Application_Sleep)
		.addFunction("OpenURL", ComponentManager::Application_OpenURL)
		.addFunction("GetFrame", Helper::GetFrameNumber)
		.endNamespace();
	// register the input functions
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Input")
		.addFunction("GetKeyDown", Input::GetKeyDown_S)
		.addFunction("GetKeyUp", Input::GetKeyUp_S)
		.addFunction("GetKey", Input::GetKey_S)
		.addFunction("GetMouseButton", Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", Input::GetMouseButtonUp)
		.addFunction("GetMousePosition", Input::GetMousePosition)
		.addFunction("GetMouseScrollDelta", Input::GetMouseWheelDelta)
		.endNamespace();
	// register glm::vec2
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();
    // register image functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Image")
		.addFunction("Preload", ImageLoader::PreloadImage)
		.addFunction("PreloadAll", ImageLoader::PreloadAllImages)
		.addFunction("Unload", ImageLoader::UnloadImage)
        .addFunction("DrawUI", Renderer::Lua_DrawUI)
        .addFunction("DrawUIEx", Renderer::Lua_DrawUIEx)
        .addFunction("Draw", Renderer::Lua_DrawImage)
        .addFunction("DrawEx", Renderer::Lua_DrawImageEx)
        .addFunction("DrawPixel", Renderer::Lua_DrawPixel)
        .endNamespace();
	// register text functions
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", TextLoader::Lua_DrawText)
		.endNamespace();
	// register audio functions
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", AudioLoader::Lua_PlayAudio)
		.addFunction("Halt", AudioLoader::Lua_HaltAudio)
		.addFunction("SetVolume", AudioLoader::Lua_SetChannelVolume)
		.endNamespace();
	// register the camera functions
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", Renderer::Lua_SetCameraCenter)
		.addFunction("GetPositionX", Renderer::Lua_GetCameraCenterX)
		.addFunction("GetPositionY", Renderer::Lua_GetCameraCenterY)
		.addFunction("SetZoom", Renderer::Lua_SetCameraZoom)
		.addFunction("GetZoom", Renderer::Lua_GetCameraZoom)
		.endNamespace();
	// scene
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", Engine::LoadScene)
		.addFunction("GetCurrent", Engine::GetCurrentScene)
		.addFunction("DontDestroy", Engine::DontDestroyOnLoad)
		.endNamespace();
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void(*) (float, float)>()
		.addProperty("x", &b2Vec2::x)
		.addProperty("y", &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.addStaticFunction("Distance", b2Distance)
		.addStaticFunction("Dot", static_cast<float(*)(const b2Vec2&, const b2Vec2&)>(b2Dot))
        .endClass();
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Rigidbody>("Rigidbody")
        .addProperty("key", &Rigidbody::key)
        .addProperty("type", &Rigidbody::type)
        .addProperty("actor", &Rigidbody::actor)
        .addProperty("enabled", &Rigidbody::enabled)
        .addProperty("x", &Rigidbody::x)
        .addProperty("y", &Rigidbody::y)
        .addProperty("body_type", &Rigidbody::body_type)
        .addProperty("precise", &Rigidbody::precise)
        .addProperty("gravity_scale", &Rigidbody::gravity_scale)
        .addProperty("density", &Rigidbody::density)
        .addProperty("angular_friction", &Rigidbody::angular_friction)
        .addProperty("rotation", &Rigidbody::rotation)
        .addProperty("has_collider", &Rigidbody::has_collider)
        .addProperty("has_trigger", &Rigidbody::has_trigger)
		.addProperty("collider_type", &Rigidbody::collider_type)
		.addProperty("width", &Rigidbody::width)
		.addProperty("height", &Rigidbody::height)
		.addProperty("radius", &Rigidbody::collider_radius)
		.addProperty("friction", &Rigidbody::friction)
		.addProperty("bounciness", &Rigidbody::bounciness)
		.addProperty("trigger_type", &Rigidbody::trigger_type)
		.addProperty("trigger_width", &Rigidbody::trigger_width)
		.addProperty("trigger_height", &Rigidbody::trigger_height)
		.addProperty("trigger_radius", &Rigidbody::trigger_radius)
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("OnStart", &Rigidbody::OnStart)
        .addFunction("OnDestroy", &Rigidbody::OnDestroy)
        .addFunction("Ready", &Rigidbody::Ready)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Collision>("Collision")
		.addData("other", &Collision::other)
		.addData("normal", &Collision::normal)
		.addData("point", &Collision::point)
		.addData("relative_velocity", &Collision::relative_velocity)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", Physics::Raycast)
		.addFunction("RaycastAll", Physics::RaycastAll)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<HitResult>("HitResult")
		.addData("actor", &HitResult::actor)
		.addData("point", &HitResult::point)
		.addData("normal", &HitResult::normal)
		.addData("is_trigger", &HitResult::is_trigger)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Event")
		.addFunction("Publish", EventBus::Publish)
		.addFunction("Subscribe", EventBus::Subscribe)
		.addFunction("Unsubscribe", EventBus::Unsubscribe)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Animation")
		.addFunction("Preload", AnimationManager::PreloadAnimation)
		.addFunction("PreloadAll", AnimationManager::PreloadAllAnimations)
		.addFunction("Unload", AnimationManager::UnloadAnimation)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Animator>("Animator")
		.addProperty("key", &Animator::key)
		.addProperty("type", &Animator::type)
		.addProperty("actor", &Animator::actor)
		.addProperty("enabled", &Animator::enabled)
		.addProperty("frames_per_second", &Animator::frames_per_second)
		.addProperty("default_animation", &Animator::lua_default_animation)
		.addFunction("OnStart", &Animator::OnStart)
		.addFunction("OnUpdate", &Animator::OnUpdate)
		.addFunction("OnDestroy", &Animator::OnDestroy)
		.addFunction("GetCurrentFrame", &Animator::GetCurrentFrame)
		.addFunction("GetCurrentAnimation", &Animator::GetCurrentAnimation)
		.addFunction("PlayAnimation", &Animator::PlayAnimation)
		.addFunction("PlayAnimationAtFPS", &Animator::PlayAnimationAtFPS)
		.addFunction("StopAnimation", &Animator::StopAnimation)
		.addFunction("SetDefaultAnimation", &Animator::SetDefaultAnimation)
		.addFunction("RemoveDefaultAnimation", &Animator::RemoveDefaultAnimation)
		.addFunction("SetFramesPerSecond", &Animator::SetFramesPerSecond)
        .addFunction("GetCallbackKey", &Animator::GetCallbackKey)
		.endClass();
}

void ComponentManager::InitializeComponents() {
	// load all the components from the components directory (if it exists)
	if(!EngineUtils::DirectoryExists("resources/component_types"))
	{
		return;
	}
	std::vector<std::string> component_files = EngineUtils::GetFilesInDirectory("resources/component_types");
	for (auto& file : component_files)
	{
		// load the component file

		if(luaL_dofile(lua_state, file.c_str()) != LUA_OK)
		{
			std::filesystem::path filePath(file);
			std::cout << "problem with lua file " << filePath.filename().stem().string();
			exit(0);
		}
	}
}

void ComponentManager::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table) {
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;

	instance_table.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, 1);
}

luabridge::LuaRef ComponentManager::CreateComponent(std::string component_type){
    luabridge::LuaRef new_table = luabridge::newTable(lua_state);
    
    // get lua files
    luabridge::LuaRef base_table = luabridge::getGlobal(lua_state, component_type.c_str());

	if(base_table.isNil()){
		std::cout << "error: failed to locate component " << component_type;
		exit(0);
	}
    
    EstablishInheritance(new_table, base_table);

    new_table["type"] = component_type;
	new_table["enabled"] = true;
    
    return new_table;
}

luabridge::LuaRef ComponentManager::CreateCppComponent(std::string component_type){
	if(component_type != "Rigidbody" && component_type != "Animator"){
		std::cout << "error: failed to locate component " << component_type;
		exit(0);
	}
	// switch case for each component type
	if(component_type == "Rigidbody"){
		Rigidbody* rb = new Rigidbody();
			// push to lua
		luabridge::LuaRef component(lua_state, &rb);
        return component;
	}
	else if(component_type == "Animator"){
        Animator* an = new Animator();
            // push to lua
        luabridge::LuaRef component(lua_state, &an);
        return component;
	}

//	if(component["Ready"].isFunction()){
//		component["Ready"](component);
//	}
	
	return luabridge::LuaRef(lua_state);
}

luabridge::LuaRef ComponentManager::CreateEmptyTable(){
    
    luabridge::LuaRef new_table = luabridge::newTable(lua_state);
    return new_table;
}

void ComponentManager::Application_OpenURL(const std::string& url){
	std::string command;

	#ifdef _WIN32
		command = "start " + url;
	#elif __APPLE__
		command = "open " + url;
	#else
		command = "xdg-open " + url;
	#endif

	std::system(command.c_str());
}

