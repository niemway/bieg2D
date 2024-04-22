//
//  Rigidbody.cpp
//  game_engine
//
//  Created by Nicholas Way on 3/25/24.
//

#include "Rigidbody.hpp"

void Rigidbody::Ready(){
	if(!world_initialized){
		world = new b2World(b2Vec2(0.0f, 9.8f));
		world->SetContactListener(&collision_detector);
		world_initialized = true;
	}

    CreateBody();
    if(has_collider)
        CreateColliderFixture();
    if(has_trigger)
        CreateTriggerFixture();
    
    if(!has_trigger && !has_collider)
        CreateDefaultFixture();
}

b2Vec2 Rigidbody::GetPosition(){
    if(rb == nullptr){
        return b2Vec2(x, y);
    }
	return rb->GetPosition();
}

float Rigidbody::GetRotation(){
    if(rb == nullptr){
        return rotation;
    }
	// return rb->GetAngle(); and convert to degrees
	return rb->GetAngle() * (180.0f / b2_pi);
}

void Rigidbody::OnStart(){
    Ready();
	return;
}

void Rigidbody::OnDestroy(){
    // destroy all fixtures
    b2Fixture* fixture = rb->GetFixtureList();
    while(fixture != nullptr){
        b2Fixture* next = fixture->GetNext();
        rb->DestroyFixture(fixture);
        fixture = next;
    }
    world->DestroyBody(rb);
}

// ------------------------ runtime physics functions ------------------------

void Rigidbody::AddForce(b2Vec2 force){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": AddForce() called before Rigidbody began simulating, please call next frame");
        return;
    }
	rb->ApplyForceToCenter(force, true);
}

void Rigidbody::SetVelocity(b2Vec2 velocity){
    if(rb == nullptr){
        starting_velocity = velocity;
        return;
    }
	rb->SetLinearVelocity(velocity);
}

void Rigidbody::SetPosition(b2Vec2 position){
    if(rb == nullptr){
        x = position.x;
        y = position.y;
        return;
    }
	rb->SetTransform(position, rb->GetAngle());
}

void Rigidbody::SetRotation(float degrees_clockwise){
    if(rb == nullptr){
        rotation = degrees_clockwise;
        return;
    }
	rb->SetTransform(rb->GetPosition(), degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": SetAngularVelocity() called before Rigidbody began simulating, please call next frame");
        return;
    }
	rb->SetAngularVelocity(degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetGravityScale(float scale){
    if(rb == nullptr){
        gravity_scale = scale;
        return;
    }
	rb->SetGravityScale(scale);
}

void Rigidbody::SetUpDirection(b2Vec2 direction){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": SetUpDirection() called before Rigidbody began simulating, please call next frame");
        return;
    }
	direction.Normalize();
	rb->SetTransform(rb->GetPosition(), glm::atan(direction.x, -direction.y));
}

void Rigidbody::SetRightDirection(b2Vec2 direction){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": SetRightDirection() called before Rigidbody began simulating, please call next frame");
        return;
    }
	direction.Normalize();
	rb->SetTransform(rb->GetPosition(), glm::atan(direction.x, -direction.y) - (b2_pi / 2.0f));
}

// ------------------------ b2body getter functions ------------------------

b2Vec2 Rigidbody::GetVelocity(){
    if(rb == nullptr){
        return starting_velocity;
    }
    return rb->GetLinearVelocity();
}

float Rigidbody::GetAngularVelocity(){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": GetAngularVelocity() called before Rigidbody began simulating, please call next frame");
        return 0.0f;
    }
    return rb->GetAngularVelocity();
}

float Rigidbody::GetGravityScale(){
    if(rb == nullptr){
        return gravity_scale;
    }
    return rb->GetGravityScale();
}

b2Vec2 Rigidbody::GetUpDirection(){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": GetUpDirection() called before Rigidbody began simulating, please call next frame");
        return b2Vec2(0.0f, 0.0f);
    }
    b2Vec2 result = b2Vec2(glm::sin(rb->GetAngle()), -glm::cos(rb->GetAngle()));
    result.Normalize();
    return result;
}

b2Vec2 Rigidbody::GetRightDirection(){
    if(rb == nullptr){
        EngineUtils::ReportError("WARNING: frame " + std::to_string(Helper::GetFrameNumber()) + ": GetUpDirection() called before Rigidbody began simulating, please call next frame");
        return b2Vec2(0.0f, 0.0f);
    }
    b2Vec2 result = b2Vec2(glm::cos(rb->GetAngle()), glm::sin(rb->GetAngle()));
    result.Normalize();
    return result;
}

void Rigidbody::CreateBody(){
	b2BodyDef body_def;
	body_def.position = b2Vec2(x, y);
    body_def.linearVelocity = starting_velocity;
    body_def.angle = rotation * (b2_pi / 180.0f);
	if(body_type == "dynamic"){
		body_def.type = b2_dynamicBody;
	}
	else if(body_type == "static"){
		body_def.type = b2_staticBody;
	}
	else if(body_type == "kinematic"){
		body_def.type = b2_kinematicBody;
	}
	body_def.bullet = precise;
	body_def.angularDamping = angular_friction;
    body_def.gravityScale = gravity_scale;

	rb = world->CreateBody(&body_def);
}

void Rigidbody::CreateColliderFixture(){
	b2Shape* shape = nullptr;
	if(collider_type == "box"){
		b2PolygonShape* box_shape = new b2PolygonShape();
		box_shape->SetAsBox(width * 0.5f, height * 0.5f);
		shape = box_shape;
	}
	else if(collider_type == "circle"){
		b2CircleShape* circle_shape = new b2CircleShape();
		circle_shape->m_radius = collider_radius;
		shape = circle_shape;
	}

	b2FixtureDef fixture;
	fixture.shape = shape;
	fixture.density = density;
	fixture.friction = friction;
	fixture.isSensor = false;
	fixture.restitution = bounciness;

	fixture.userData.pointer = reinterpret_cast<uintptr_t>(this);

	rb->CreateFixture(&fixture);
}

void Rigidbody::CreateTriggerFixture(){
	b2Shape* shape = nullptr;
	if(trigger_type == "box"){
		b2PolygonShape* box_shape = new b2PolygonShape();
		box_shape->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
		shape = box_shape;
	}
	else if(trigger_type == "circle"){
		b2CircleShape* circle_shape = new b2CircleShape();
		circle_shape->m_radius = trigger_radius;
		shape = circle_shape;
	}

	b2FixtureDef fixture;
	fixture.shape = shape;
	fixture.density = density;
	fixture.friction = friction;
	fixture.isSensor = true;
	fixture.restitution = bounciness;

	fixture.userData.pointer = reinterpret_cast<uintptr_t>(this);

	rb->CreateFixture(&fixture);
}

void Rigidbody::CreateDefaultFixture(){
    b2PolygonShape phantom_shape;
    phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);
    b2FixtureDef phantom_fixture_def;
    phantom_fixture_def.shape = &phantom_shape;
    phantom_fixture_def.density = density;
    // Because it is a sensor (with no callback even), no collisions will ever occur
    phantom_fixture_def.isSensor = true;

//	phantom_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

    rb->CreateFixture(&phantom_fixture_def);
}
