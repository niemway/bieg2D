//
//  Actor.hpp
//  game_engine
//
//  Created by Nicholas Way on 1/30/24.
//

#ifndef Actor_hpp
#define Actor_hpp

#include <stdio.h>
#include <string>
#include <optional>
#include "glm/glm.hpp"
#include "ImageLoader.h"
#include "AudioLoader.h"
#include "ComponentManager.hpp"
#include "Collision.hpp"

//#ifdef _WIN32
#include <SDL.h>
//#else
//#include <SDL2/SDL.h>
//#endif

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Collision;

class Actor
{
public:
	int actor_uid;
    
    std::string name;

	bool marked_for_deletion = false;
    
    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components;

	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_to_start;
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_to_update;
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_to_late_update;
	std::vector<std::shared_ptr<luabridge::LuaRef>> components_to_add;
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_to_remove;

	Actor()
	{
		actor_uid = -1;
		name = "";
	}
    
    void Start();
    void Update();
    void LateUpdate();
    void Destroy();

	void InjectAllConvenienceReferences();
	void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref);

	void EnableAllComponents();
	void DisableAllComponents();

	// physics
    void OnCollision(Collision* collision, std::string type);

	// Functions Accessible to Lua
	std::string GetName() { return name; }
	int GetID() { return actor_uid; }
	luabridge::LuaRef GetComponentByKey(const std::string& key);
	luabridge::LuaRef GetComponent(const std::string& type);
	luabridge::LuaRef GetComponents(const std::string& type);

	luabridge::LuaRef AddComponent(const std::string& type);
	void RemoveComponent(luabridge::LuaRef component);

	void ParseAllLifecycleFunctions();
	void ParseLifecycleFunctions(std::shared_ptr<luabridge::LuaRef> component);

	static inline int num_added_components = 0;
};

#endif /* Actor_hpp */
