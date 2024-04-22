//
//  ComponentManager.hpp
//  game_engine
//
//  Created by Nicholas Way on 3/7/24.
//

#ifndef ComponentManager_hpp
#define ComponentManager_hpp

#include <stdio.h>
#include <vector>
#include <filesystem>
#include <thread>

#include "Component.hpp"
#include "EngineUtils.h"
#include "InputManager.hpp"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "box2d/box2d.h"

class ComponentManager
{
public:
	static void Initialize();
    static void InitializeState();
    static void InitializeFunctions();
    static void InitializeComponents();

	static lua_State* GetLuaState() { return lua_state; }

	static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);
    static luabridge::LuaRef CreateComponent(std::string component_name);
	static luabridge::LuaRef CreateCppComponent(std::string component_name);
    static luabridge::LuaRef CreateEmptyTable();

	static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> component_tables;
    
	// Debug Namespace
    static void Print(const std::string& message) { std::cout << message << std::endl; }
    static void PrintError(const std::string& message) { std::cerr << message << std::endl; }
	// Application Namespace
	static void Application_Quit() { exit(0); }
	static void Application_Sleep(int milliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); }
	static void Application_OpenURL(const std::string& url);

private:
	static inline lua_State* lua_state;
};

#endif /* ComponentManager_hpp */
