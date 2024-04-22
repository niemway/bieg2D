//
//  Rigidbody.hpp
//  game_engine
//
//  Created by Nicholas Way on 3/25/24.
//

#ifndef Rigidbody_hpp
#define Rigidbody_hpp

#include <stdio.h>
#include <string>

#include "Actor.hpp"
#include "Collision.hpp"

#include "box2d/box2d.h"

class Rigidbody
{
public:

    static inline bool world_initialized;
    static inline b2World* world;
    // contact listener
    static inline CollisionDetector collision_detector;

    void Ready();

	// generic component data
	std::string type = "Rigidbody";
	std::string key = "";
    Actor* actor = nullptr;
	bool enabled = true;
    
    b2Body* rb;

	float x = 0.0f;
	float y = 0.0f;
	std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0f;
    float density = 1.0f;
    float angular_friction = 0.3f;
    float rotation = 0.0f;
    b2Vec2 starting_velocity = b2Vec2(0,0);
    
    // collider properties
    bool has_collider = true;
    std::string collider_type = "box";
    float width = 1.0f;
    float height = 1.0f;
    float collider_radius = 0.5f;
    float friction = 0.3f;
    float bounciness = 0.3f;

    // trigger properties
    bool has_trigger = true;
    std::string trigger_type = "box";
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;
    
    b2Vec2 GetPosition();
    float GetRotation();
    
    void OnStart();
    void OnDestroy();

    // runtime functions
    void AddForce(b2Vec2 force);
    void SetVelocity(b2Vec2 velocity);
    void SetPosition(b2Vec2 position);
    void SetRotation(float degrees_clockwise);
    void SetAngularVelocity(float degrees_clockwise);
    void SetGravityScale(float scale);
    void SetUpDirection(b2Vec2 direction);
    void SetRightDirection(b2Vec2 direction);
    
    b2Vec2 GetVelocity();
    float GetAngularVelocity();
    float GetGravityScale();
    b2Vec2 GetUpDirection();
    b2Vec2 GetRightDirection();

private:

    void CreateBody();
    void CreateColliderFixture();
    void CreateTriggerFixture();
    void CreateDefaultFixture();
};

#endif /* Rigidbody_hpp */
