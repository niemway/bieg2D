//
//  Physics.cpp
//  game_engine
//
//  Created by Nicholas Way on 3/31/24.
//

#include "Physics.hpp"

float RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction){
	HitResult hit_result;
	Rigidbody* rb = reinterpret_cast<Rigidbody*>(fixture->GetUserData().pointer);
    if(rb == nullptr) return -1.0f;
    hit_result.actor = rb->actor;
	hit_result.point = point;
	hit_result.normal = normal;
    hit_result.fraction_from_origin = fraction;
	hit_result.is_trigger = fixture->IsSensor();
    
    if(hit_result.actor == nullptr) return -1.0f;
    
	hit_results.push_back(hit_result);
	return 1.0f;
}

luabridge::LuaRef Physics::Raycast(b2Vec2 pos, b2Vec2 dir, float dist){
    if(!Rigidbody::world_initialized){
        return luabridge::LuaRef(ComponentManager::GetLuaState());
    }
    
    dir.Normalize();
    
    RaycastCallback callback;
    Rigidbody::world->RayCast(&callback, pos, pos + (dist * dir));

    std::stable_sort(callback.hit_results.begin(), callback.hit_results.end(), [](HitResult a, HitResult b){
        return a.fraction_from_origin < b.fraction_from_origin;
    });

    if(callback.hit_results.size() > 0){
        return luabridge::LuaRef(ComponentManager::GetLuaState(), callback.hit_results.front());
    }else{
        return luabridge::LuaRef(ComponentManager::GetLuaState());
    }
}

luabridge::LuaRef Physics::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist){
    if(!Rigidbody::world_initialized){
        return luabridge::LuaRef(ComponentManager::GetLuaState());
    }
    
    dir.Normalize();
    
    RaycastCallback callback;
    Rigidbody::world->RayCast(&callback, pos, pos + (dist * dir));

    std::stable_sort(callback.hit_results.begin(), callback.hit_results.end(), [](HitResult a, HitResult b){
        return a.fraction_from_origin < b.fraction_from_origin;
    });

    luabridge::LuaRef results = luabridge::newTable(ComponentManager::GetLuaState());
    for(int i = 0; i < callback.hit_results.size(); i++){
        results[i + 1] = luabridge::LuaRef(ComponentManager::GetLuaState(), callback.hit_results[i]);
    }

    return results;
}
