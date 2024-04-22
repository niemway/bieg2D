//
//  Collision.cpp
//  game_engine
//
//  Created by Nicholas Way on 3/30/24.
//

#include "Collision.hpp"
#include "Rigidbody.hpp"

void CollisionDetector::BeginContact(b2Contact* contact){
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    Rigidbody* rbA = reinterpret_cast<Rigidbody*>(fixtureA->GetUserData().pointer);
    Rigidbody* rbB = reinterpret_cast<Rigidbody*>(fixtureB->GetUserData().pointer);

    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);

    // check if either actor is invalid
    if(rbA == nullptr || rbB == nullptr){
        return;
    }
    
    Collision collision = Collision(rbB->actor, world_manifold.points[0], rbA->GetVelocity() - rbB->GetVelocity(), world_manifold.normal);

    if(fixtureA->IsSensor() && fixtureB->IsSensor()){
        if(!(rbA->has_trigger && rbB->has_trigger)) return;
        
        collision.point = b2Vec2(-999.0f, -999.0f);
        collision.normal = b2Vec2(-999.0f, -999.0f);
        rbA->actor->OnCollision(&collision, "OnTriggerEnter");
        collision.other = rbA->actor;
        rbB->actor->OnCollision(&collision, "OnTriggerEnter");
    }
    else if(!fixtureA->IsSensor() && !fixtureB->IsSensor()){
        rbA->actor->OnCollision(&collision, "OnCollisionEnter");
        collision.other = rbA->actor;
        rbB->actor->OnCollision(&collision, "OnCollisionEnter");
    }
}

void CollisionDetector::EndContact(b2Contact* contact){
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    Rigidbody* rbA = reinterpret_cast<Rigidbody*>(fixtureA->GetUserData().pointer);
    Rigidbody* rbB = reinterpret_cast<Rigidbody*>(fixtureB->GetUserData().pointer);
    
    // check if either actor is invalid
    if(rbA == nullptr || rbB == nullptr){
        return;
    }

    Collision collision = Collision(rbB->actor, b2Vec2(-999.0f, -999.0f), rbA->GetVelocity() - rbB->GetVelocity(), b2Vec2(-999.0f, -999.0f));

    // check if either actor is invalid
    if(rbA == nullptr || rbB == nullptr){
        std::cerr << "Invalid Rigidbody reference in CollisionDetector::EndContact" << std::endl;
        return;
    }

    if(fixtureA->IsSensor() && fixtureB->IsSensor()){
        if(!(rbA->has_trigger && rbB->has_trigger)) return;
        
        rbA->actor->OnCollision(&collision, "OnTriggerExit");
        collision.other = rbA->actor;
        rbB->actor->OnCollision(&collision, "OnTriggerExit");
    }
    else if(!fixtureA->IsSensor() && !fixtureB->IsSensor()){
        rbA->actor->OnCollision(&collision, "OnCollisionExit");
        collision.other = rbA->actor;
        rbB->actor->OnCollision(&collision, "OnCollisionExit");
    }
}
