//
//  Animation.hpp
//  game_engine
//
//  Created by Nicholas Way on 4/11/24.
//

#ifndef Animation_hpp
#define Animation_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <optional>
#include <map>

#include "ImageLoader.h"

struct Animation
{
	std::string name = "";
	std::vector<std::string> frames;
	std::optional<float> frames_per_second;

	void PreloadFrames();
	void UnloadFrames();
};

class AnimationManager
{
	// map storing all loaded animations
	static inline std::map<std::string, Animation> animations;

public:
	// load an animation from a file
	static void LoadAnimation(const std::string& name);
	// get an animation by name
	static Animation* GetAnimation(const std::string& name);

	static void PreloadAnimation(const std::string& name);
	static void PreloadAllAnimations();
	static void UnloadAnimation(const std::string& name);
};

#endif /* Animation_hpp */
