//
//  Physics.hpp
//  game_engine
//
//  Created by Nicholas Way on 3/31/24.
//

#ifndef Physics_hpp
#define Physics_hpp

#include <stdio.h>
#include <vector>

#include "Actor.hpp"
#include "Rigidbody.hpp"

#include "box2d/box2d.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

struct HitResult{
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger;

    float fraction_from_origin;
};

class Physics{
public:
    // Physics.Raycast
    static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float distance);
    static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float distance);
};

class RaycastCallback : public b2RayCastCallback{
public:
    std::vector<HitResult> hit_results;
    
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
};

#endif /* Physics_hpp */
