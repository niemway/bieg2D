//
//  SceneLoader.hpp
//  game_engine
//
//  Created by Nicholas Way on 1/30/24.
//

#ifndef SceneLoader_hpp
#define SceneLoader_hpp

#ifdef _WIN32
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include <stdio.h>
#include <iostream>
#include <vector>
#include "EngineUtils.h"
#include "rapidjson/document.h"
#include "Actor.hpp"
#include "ActorTemplateManager.hpp"
#include "ImageLoader.h"
#include "ComponentManager.hpp"
#include "Engine.hpp"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class SceneLoader
{
public:
	static inline int next_actor_uid = 0;

	// load a scene from a json file, and add actors to the actors vector
	static void LoadScene(const std::string& name, Engine* engine){
		// std::string path = "resources/scenes/" + name + ".scene";
		// If a scene may be loaded because no scene with the specified name exists, print an error.
		if (!EngineUtils::DirectoryExists("resources/scenes/" + name + ".scene")){
			std::cout << "error: scene " << name << " is missing";
			exit(0);
		}
		rapidjson::Document out_document;
		EngineUtils::ReadJsonFile("resources/scenes/" + name + ".scene", out_document);
		// get the actors array
		const rapidjson::Value& actors_array = out_document["actors"];
		engine->ClearActors();
		engine->AddDontDestroyOnLoadActors();
		// iterate through the actors array
		for (rapidjson::SizeType i = 0; i < actors_array.Size(); i++)
		{
			const rapidjson::Value& actor = actors_array[i];
			Actor new_actor;
			// check if the actor is using a template
			if (actor.HasMember("template")){
				std::string template_name = actor["template"].GetString();
				Actor template_actor = ActorTemplateManager::GetActorTemplate(template_name);
				new_actor.name = template_actor.name;
				// copy the template actor's components to the new actor
				for(auto& component : template_actor.components){
					// using establish inheritance to copy the component
                    /*
                    luabridge::LuaRef new_table = ComponentManager::CreateComponent(component.second->operator[]("type").cast<std::string>());
					luabridge::LuaRef template_table = *component.second.get();
					ComponentManager::EstablishInheritance(template_table, new_table);
                    if(new_table["OnStart"].isFunction()){
                        std::cout << "now";
                    }
                    */
					new_actor.components[component.first] = component.second;
				}
			}
			new_actor.actor_uid = next_actor_uid++;
			if(actor.HasMember("name"))
				new_actor.name = actor["name"].GetString();
            
            // load in components
            if(actor.HasMember("components")){
                
                const rapidjson::Value& c = actor["components"];
                for(auto iter = c.MemberBegin(); iter != c.MemberEnd(); iter++){
                    std::string componentName = iter->name.GetString();
                    std::string componentType;
					if(iter->value.HasMember("type"))
                        componentType = iter->value["type"].GetString();
					else
						componentType = new_actor.components[componentName]->operator[]("type").cast<std::string>();

					std::shared_ptr<luabridge::LuaRef> new_component_ptr;

					// if the key is not already in the map, create a new component. Otherwise, use the template
					if (new_actor.components.find(componentName) == new_actor.components.end()){

						if(componentType == "Rigidbody" || componentType == "Animator"){
							new_component_ptr = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateCppComponent(componentType));
						}
						else{
							new_component_ptr = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateComponent(componentType));
						}
						// new_component_ptr = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateComponent(componentType));

						// add key variable to component
						(*new_component_ptr)["key"] = componentName;

					}
					else{
						new_component_ptr = new_actor.components[componentName];
					}


//					luabridge::LuaRef new_component = *new_component_ptr;
                    
                    for(auto propertyIter = iter->value.MemberBegin(); propertyIter != iter->value.MemberEnd(); propertyIter++){
                        std::string propertyName = propertyIter->name.GetString();
                        
                        if(propertyIter->value.IsString()){
                            // add string property
							(*new_component_ptr)[propertyName] = propertyIter->value.GetString();
                        }
						else if(propertyIter->value.IsInt()){
                            (*new_component_ptr)[propertyName] = propertyIter->value.GetInt();
						}
						else if(propertyIter->value.IsFloat()){
                            (*new_component_ptr)[propertyName] = propertyIter->value.GetFloat();
						}
						else if(propertyIter->value.IsBool()){
                            (*new_component_ptr)[propertyName] = propertyIter->value.GetBool();
						}
                        
                    }
                    
                    // insert component into map
                    new_actor.components[iter->name.GetString()] = new_component_ptr;
                }
            }

			// make shared pointer to new actor
			std::shared_ptr<Actor> new_actor_ptr = std::make_shared<Actor>(new_actor);

			engine->AddActor(new_actor_ptr);
		}
	}
};

#endif /* SceneLoader_hpp */
