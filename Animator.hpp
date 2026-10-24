//
//  Animator.hpp
//  game_engine
//
//  Created by Nicholas Way on 4/11/24.
//

#ifndef Animator_hpp
#define Animator_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <optional>

#include "Animation.hpp"
#include "Renderer.hpp"
#include "EventBus.hpp"

class Animator
{
public:

	// generic component data
	std::string type = "Animator";
	std::string key = "";
    Actor* actor = nullptr;
	bool enabled = true;
    std::optional<std::string> callback_key;

	float frames_per_second = 8.0f;
	// looping animation that plays when no other animations are playing
	std::optional<std::string> default_animation;
    std::string lua_default_animation = "";
	// current animation
	std::optional<std::string> current_animation;

	void OnStart();
	void OnUpdate();
	void OnDestroy();

	const std::string& GetCurrentFrame();
	std::string GetCurrentAnimation();

	void PlayAnimation(const std::string& name);
	void PlayAnimationAtFPS(const std::string& name, float fps);
	void StopAnimation();
	void SetDefaultAnimation(const std::string& name);
	void RemoveDefaultAnimation();

	void SetFramesPerSecond(float fps);
    std::string GetCallbackKey();
private:
	float current_fps = 8.0f;
	float game_frames_per_animation_frame = 60.0f/8.0f;

	// animation currently driving playback (current_animation, else default_animation)
	// AnimationManager never erases entries, so this pointer stays valid
	Animation* active_animation = nullptr;

	int current_frame_index = 0;
	std::string current_frame = "";
	// game frames elapsed in the current animation frame, keeps the fractional remainder
	float frames_since_last_frame = 0.0f;

	void ReturnToDefaultAnimation();
    void PerformCallback();
};

#endif /* Animator_hpp */
