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
	// a script may have already started an animation before this OnStart ran
	bool already_playing = current_animation.has_value();
	// use the frames_per_second property unless the animation specifies its own
	if(!already_playing) {
		SetFramesPerSecond(frames_per_second);
	}
	// preload the default animation
	if(default_animation.has_value()) {
		Animation* animation = AnimationManager::GetAnimation(default_animation.value());
		animation->PreloadFrames();
		if(!already_playing) {
			active_animation = animation;
			current_frame = animation->frames[current_frame_index];
			if(animation->frames_per_second.has_value()) {
				SetFramesPerSecond(animation->frames_per_second.value());
			}
		}
	}
}

void Animator::OnUpdate() {
	if(active_animation == nullptr) {
		return;
	}
	// a non-positive or NaN rate pauses playback
	if(!(game_frames_per_animation_frame > 0.0f)) {
		return;
	}
	// check if the frame should be incremented
	frames_since_last_frame += 1.0f;
	while(frames_since_last_frame >= game_frames_per_animation_frame) {
		frames_since_last_frame -= game_frames_per_animation_frame;
		current_frame_index++;
		if(current_frame_index >= active_animation->frames.size()) {
			// the default animation loops without a callback, so carry the remainder into the next loop
			bool looping_default = !current_animation.has_value();
			float remainder = frames_since_last_frame;
			StopAnimation();
			if(looping_default) {
				frames_since_last_frame = remainder;
			}
			break;
		}
		current_frame = active_animation->frames[current_frame_index];
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
	active_animation = nullptr;
}

const std::string& Animator::GetCurrentFrame() {
	return current_frame;
}

std::string Animator::GetCurrentAnimation() {
	return current_animation.value_or(default_animation.value_or(""));
}

void Animator::PlayAnimation(const std::string& name) {
	Animation* animation = AnimationManager::GetAnimation(name);
	// make sure frames are loaded now rather than mid-playback
	animation->PreloadFrames();
	if(animation->frames_per_second.has_value()) {
		SetFramesPerSecond(animation->frames_per_second.value());
	}
	current_animation = name;
	active_animation = animation;
	current_frame_index = 0;
	frames_since_last_frame = 0.0f;
	current_frame = animation->frames[current_frame_index];
}

void Animator::PlayAnimationAtFPS(const std::string& name, float fps) {
	PlayAnimation(name);
	SetFramesPerSecond(fps);
}

void Animator::StopAnimation() {
	bool was_playing = current_animation.has_value();
	current_animation.reset();
	current_frame_index = 0;
	frames_since_last_frame = 0.0f;
	if(default_animation.has_value()) {
		Animation* animation = AnimationManager::GetAnimation(default_animation.value());
		active_animation = animation;
		current_frame = animation->frames[current_frame_index];
        SetFramesPerSecond(animation->frames_per_second.value_or(frames_per_second));
	}
	else {
		active_animation = nullptr;
		current_frame = "";
	}
	// publish after resetting so a callback can start another animation
	if(was_playing) {
		PerformCallback();
	}
}

void Animator::SetDefaultAnimation(const std::string& name) {
	default_animation = name;
	if(!current_animation.has_value()) {
		active_animation = AnimationManager::GetAnimation(name);
	}
}

void Animator::RemoveDefaultAnimation() {
	default_animation.reset();
	if(!current_animation.has_value()) {
		active_animation = nullptr;
	}
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
