//
//  Collision.hpp
//  game_engine
//
//  Created by Nicholas Way on 3/30/24.
//

#ifndef Collision_hpp
#define Collision_hpp

#include <stdio.h>

#include "box2d/box2d.h"
#include "Actor.hpp"
//#include "Rigidbody.hpp"

class CollisionDetector : public b2ContactListener
{
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
};

class Actor;

class Collision
{
public:
    Actor* other = nullptr;
    b2Vec2 point = b2Vec2(0.0f, 0.0f);
    b2Vec2 relative_velocity = b2Vec2(0.0f, 0.0f);
    b2Vec2 normal = b2Vec2(0.0f, 0.0f);

    Collision() {}
    Collision(Actor* other, b2Vec2 point, b2Vec2 relative_velocity, b2Vec2 normal) : other(other), point(point), relative_velocity(relative_velocity), normal(normal) {}
};

#endif /* Collision_hpp */
