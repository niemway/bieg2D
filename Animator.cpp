//
//  Animator.cpp
//  game_engine
//
//  Created by Nicholas Way on 4/11/24.
//

#include "Animator.hpp"

void Animator::OnStart() {
    if(actor != nullptr){
        // build callback key with Animator, key, actor uid
		callback_key = "`~Animator" + key + std::to_string(actor->actor_uid);
    } else {
        std::cout << "Animator: actor property is invalid" << std::endl;
        exit(0);
    }
    
    if(lua_default_animation != "")
        default_animation = lua_default_animation;
	// preload the default animation
	if(default_animation.has_value()) {
		Animation* animation = AnimationManager::GetAnimation(default_animation.value());
		animation->PreloadFrames();
        current_frame = animation->frames[current_frame_index];
		if(animation->frames_per_second.has_value()) {
			SetFramesPerSecond(animation->frames_per_second.value());
		}
	}
}

void Animator::OnUpdate() {
	if(!current_animation.has_value() && !default_animation.has_value()) {
		return;
	}
	std::string animation_name = current_animation.value_or(default_animation.value());
	Animation* animation = AnimationManager::GetAnimation(animation_name);
	// check if the frame should be incremented
	frames_since_last_frame++;
	if(frames_since_last_frame >= game_frames_per_animation_frame) {
		current_frame_index++;
		if(current_frame_index >= animation->frames.size()) {
			StopAnimation();
		}
		else {
			current_frame = animation->frames[current_frame_index];
		}
		frames_since_last_frame = 0;
	}
}

void Animator::OnDestroy() {
	// unload the default animation
	if(default_animation.has_value()) {
		AnimationManager::UnloadAnimation(default_animation.value());
	}
	// unload the current animation
	if(current_animation.has_value()) {
		AnimationManager::UnloadAnimation(current_animation.value());
	}
}

std::string Animator::GetCurrentFrame() {
	return current_frame;
}

std::string Animator::GetCurrentAnimation() {
	return current_animation.value_or(default_animation.value());
}

void Animator::PlayAnimation(const std::string& name) {
	Animation* animation = AnimationManager::GetAnimation(name);
	if(animation->frames_per_second.has_value()) {
		SetFramesPerSecond(animation->frames_per_second.value());
	}
	current_animation = name;
	current_frame_index = 0;
	frames_since_last_frame = 0;
	current_frame = animation->frames[current_frame_index];
}

void Animator::PlayAnimationAtFPS(const std::string& name, float fps) {
	PlayAnimation(name);
	SetFramesPerSecond(fps);
}

void Animator::StopAnimation() {
    if(current_animation.has_value()){
        PerformCallback();
    }
	current_animation.reset();
	current_frame_index = 0;
	frames_since_last_frame = 0;
	if(default_animation.has_value()) {
		Animation* animation = AnimationManager::GetAnimation(default_animation.value());
		current_frame = animation->frames[current_frame_index];
        SetFramesPerSecond(animation->frames_per_second.value_or(frames_per_second));
	}
	else {
		current_frame = "";
	}
}

void Animator::SetDefaultAnimation(const std::string& name) {
	default_animation = name;
}

void Animator::RemoveDefaultAnimation() {
	default_animation.reset();
}

void Animator::SetFramesPerSecond(float fps) {
	current_fps = fps;
	game_frames_per_animation_frame = 60.0f/current_fps;
}

std::string Animator::GetCallbackKey() {
    return callback_key.value_or("`~Animator" + key + std::to_string(actor->actor_uid));
}

void Animator::PerformCallback() {
    EventBus::Publish_NoRef(callback_key.value());
}
