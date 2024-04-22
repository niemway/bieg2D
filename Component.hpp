//
//  Component.hpp
//  game_engine
//
//  Created by Nicholas Way on 3/7/24.
//

#ifndef Component_hpp
#define Component_hpp

#include <stdio.h>
#include <memory>

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component
{
public:
	explicit Component();

	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool value) { enabled = value; }

	std::shared_ptr<luabridge::LuaRef> componentRef;
	std::string type;

	bool hasStart = false;
	bool hasUpdate = false;
	bool hasLateUpdate = false;

private:
	bool enabled = true;

};

#endif /* Component_hpp */
