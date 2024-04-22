//
//  EventBus.hpp
//  game_engine
//
//  Created by Nicholas Way on 4/1/24.
//

#ifndef EventBus_hpp
#define EventBus_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "EngineUtils.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class EventBus
{
public:
	
	static void Publish(std::string event_name, luabridge::LuaRef event_object);
    static void Publish_NoRef(std::string event_name);
	static void Subscribe(std::string event_name, luabridge::LuaRef component, luabridge::LuaRef function);
	static void Unsubscribe(std::string event_name, luabridge::LuaRef component, luabridge::LuaRef function);

	static void ProcessPending();
	static void ProcessPendingSubscriptions();
	static void ProcessPendingUnsubscriptions();
	
	static inline std::map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> subscriptions;
	static inline std::vector<std::pair<std::string, std::pair<luabridge::LuaRef, luabridge::LuaRef>>> pending_subscriptions;
	static inline std::vector<std::pair<std::string, std::pair<luabridge::LuaRef, luabridge::LuaRef>>> pending_unsubscriptions;
};

#endif /* EventBus_hpp */
