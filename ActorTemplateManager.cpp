//
//  ActorTemplateManager.cpp
//  game_engine
//
//  Created by Nicholas Way on 1/31/24.
//

#ifdef _WIN32
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include "ActorTemplateManager.hpp"
#include "rapidjson/document.h"

Actor ActorTemplateManager::_GetActorTemplateFromPath(const std::string& path)
{
	rapidjson::Document actor;
	EngineUtils::ReadJsonFile(path, actor);
	Actor new_actor;
	if (actor.HasMember("name"))
		new_actor.name = actor["name"].GetString();
    
	// load in components
	if(actor.HasMember("components")){
		const rapidjson::Value& c = actor["components"];
		for (auto iter = c.MemberBegin(); iter != c.MemberEnd(); iter++)
		{
            std::string component_name = iter->name.GetString();
            std::string component_type = iter->value["type"].GetString();
            std::shared_ptr<luabridge::LuaRef> new_component;
			if(component_type == "Rigidbody" || component_type == "Animator"){
				new_component = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateCppComponent(component_type));
			}
			else{
				new_component = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateComponent(component_type));
			}
            
			(*new_component)["key"] = component_name;

			for(auto propertyIter = iter->value.MemberBegin(); propertyIter != iter->value.MemberEnd(); propertyIter++){
				std::string propertyName = propertyIter->name.GetString();
				
				if(propertyIter->value.IsString()){
					// add string property
					(*new_component)[propertyName] = propertyIter->value.GetString();
				}
				else if(propertyIter->value.IsInt()){
					(*new_component)[propertyName] = propertyIter->value.GetInt();
				}
				else if(propertyIter->value.IsFloat()){
					(*new_component)[propertyName] = propertyIter->value.GetFloat();
				}
				else if(propertyIter->value.IsBool()){
					(*new_component)[propertyName] = propertyIter->value.GetBool();
				}
				
			}
            
            new_actor.components[component_name] = new_component;
		}
	}

	return new_actor;
}

Actor ActorTemplateManager::GetActorTemplate(const std::string& name)
{
    if(!EngineUtils::DirectoryExists("resources/actor_templates/" + name + ".template")){
        std::cout << "error: template " << name << " is missing";
        exit(0);
    }
	return _GetActorTemplateFromPath("resources/actor_templates/" + name + ".template");
}
