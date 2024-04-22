//
//  Actor.cpp
//  game_engine
//
//  Created by Nicholas Way on 1/30/24.
//

#include "Actor.hpp"

void Actor::Start(){
    for (const auto& component : components_to_add) {
//        components[(*component)["key"].cast<std::string>()] = component;
        components[(*component)["key"].cast<std::string>()] = component;
        ParseLifecycleFunctions(component);
    }
    components_to_add.clear();
    for (const auto& component : components_to_start) {
        if((*component.second.get())["OnStart"].isFunction() && (*component.second.get())["enabled"]){
            try{
                (*component.second.get())["OnStart"](*component.second.get());
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError(name, e);
            }
        }
    }
    components_to_start.clear();
}

void Actor::Update(){

    for (const auto& component : components_to_update) {
        if((*component.second.get())["OnUpdate"].isFunction() && (*component.second.get())["enabled"]){
            try{
                (*component.second.get())["OnUpdate"](*component.second.get());
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError(name, e);
            }
        }
    }
}

void Actor::LateUpdate(){
    for (const auto& component : components_to_late_update) {
        if((*component.second.get())["OnLateUpdate"].isFunction() && (*component.second.get())["enabled"]){
            try{
                (*component.second.get())["OnLateUpdate"](*component.second.get());
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError(name, e);
            }
        }
    }
    
    // Add components
    for (const auto& component : components_to_add) {
//        components[(*component)["key"].cast<std::string>()] = component;
        components[(*component)["key"].cast<std::string>()] = component;
        ParseLifecycleFunctions(component);
    }
    components_to_add.clear();

    // Remove components
    for (const auto& component : components_to_remove) {
        // call OnDestroy
        if((*component.second)["OnDestroy"].isFunction()){
            try{
                (*component.second)["OnDestroy"](*component.second);
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError(name, e);
            }
        }
        components.erase(component.first);
    }
    components_to_remove.clear();
}

void Actor::Destroy(){
    for (const auto& component : components) {
        if((*component.second)["OnDestroy"].isFunction()){
            try{
                (*component.second)["OnDestroy"](*component.second);
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError(name, e);
            }
        }
    }
}

void Actor::InjectAllConvenienceReferences(){
    for (const auto& component : components) {
        InjectConvenienceReferences(component.second);
    }
    ParseAllLifecycleFunctions();
}

void Actor::InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref){
    (*component_ref)["actor"] = this;
}

void Actor::EnableAllComponents(){
    for (const auto& component : components) {
        (*component.second)["enabled"] = true;
    }
}

void Actor::DisableAllComponents(){
    for (const auto& component : components) {
        (*component.second)["enabled"] = false;
    }
}

void Actor::OnCollision(Collision* collision, std::string type){
    for (const auto& component : components) {
        if((*component.second)[type].isFunction() && (*component.second)["enabled"]){
            try{
                (*component.second)[type](*component.second, collision);
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError(name, e);
            }
        }
    }
}

luabridge::LuaRef Actor::GetComponentByKey(const std::string& key){
    if(components.find(key) != components.end()){
        return *components[key];
    }
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

luabridge::LuaRef Actor::GetComponent(const std::string& type){
    for (const auto& component : components) {
        std::string c_t = (*component.second)["type"].cast<std::string>();
        std::string c_k = (*component.second)["key"].cast<std::string>();
        if(components_to_remove.count(c_k) <= 0 && c_t == type){
            return *component.second;
        }
    }
    // also check the components to add
    for (const auto& component : components_to_add) {
        if((*component)["type"].cast<std::string>() == type && components_to_remove.count((*component)["key"].cast<std::string>()) <= 0){
            return *component;
        }
    }
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

luabridge::LuaRef Actor::GetComponents(const std::string& type){
    luabridge::LuaRef component_table = ComponentManager::CreateEmptyTable();
    // table should be indexed by the keys of the components
    int components_found = 0;
    for (const auto& component : components) {
        if((*component.second)["type"].cast<std::string>() == type){
            component_table[++components_found] = *component.second;
        }
    }
    return component_table;
} 

luabridge::LuaRef Actor::AddComponent(const std::string& type){
    std::shared_ptr<luabridge::LuaRef> new_component;
    if(type == "Rigidbody"){
        new_component = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateCppComponent(type));
    }
    else{
        new_component = std::make_shared<luabridge::LuaRef>(ComponentManager::CreateComponent(type));
    }
    std::string key = "r" + std::to_string(num_added_components++);
    (*new_component)["key"] = key;
    InjectConvenienceReferences(new_component);
    components_to_add.push_back(new_component);
    return *new_component;
}

void Actor::RemoveComponent(luabridge::LuaRef component){
    std::string key = component["key"].cast<std::string>();
    if(components.find(key) != components.end()){
        (*components[key])["enabled"] = false;
//        components[key] = luabridge::LuaRef(ComponentManager::GetLuaState());
        components_to_remove[key] = (components[key]);
    }
}

void Actor::ParseAllLifecycleFunctions(){
    for (const auto& component : components) {
        ParseLifecycleFunctions(component.second);
    }
}

void Actor::ParseLifecycleFunctions(std::shared_ptr<luabridge::LuaRef> component){
    if((*component)["OnStart"].isFunction()){
        components_to_start[(*component)["key"].cast<std::string>()] = component;
    }
    if((*component)["OnUpdate"].isFunction()){
        components_to_update[(*component)["key"].cast<std::string>()] = component;
    }
    if((*component)["OnLateUpdate"].isFunction()){
        components_to_late_update[(*component)["key"].cast<std::string>()] = component;
    }
}
