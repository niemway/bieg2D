//
//  Animation.cpp
//  game_engine
//
//  Created by Nicholas Way on 4/11/24.
//

#include "Animation.hpp"

void Animation::PreloadFrames() {
	// preload all the frames
	for(const std::string& frame : frames) {
		ImageLoader::PreloadImage(frame);
	}
}

void Animation::UnloadFrames() {
	// unload all the frames
	for(const std::string& frame : frames) {
		ImageLoader::UnloadImage(frame);
	}
}

void AnimationManager::LoadAnimation(const std::string& name) {
	std::string file_path = "resources/animations/" + name + ".animation";
	if(!EngineUtils::DirectoryExists(file_path)){
		std::cout << "error: animation " << name << " is missing";
		exit(0);
	}
	rapidjson::Document out_document;
	EngineUtils::ReadJsonFile(file_path, out_document);
	Animation animation;
	animation.name = name;
	if(out_document.HasMember("frames_per_second")){
		animation.frames_per_second = out_document["frames_per_second"].GetFloat();
	}
	if(!out_document.HasMember("frames")){
		std::cout << "error: animation " << name << " is missing frames";
		exit(0);
	}
	const rapidjson::Value& frames_array = out_document["frames"];
	for (rapidjson::SizeType i = 0; i < frames_array.Size(); i++)
	{
		// if value is an int, it means to hold the previous frame for that many frames
		if(frames_array[i].IsInt()) {
			int num_frames = frames_array[i].GetInt();
			for(int j = 0; j < num_frames; j++) {
				animation.frames.push_back(animation.frames[animation.frames.size() - 1]);
			}
			continue;
		}
		else {
			animation.frames.push_back(frames_array[i].GetString());
		}
	}
	animations[animation.name] = animation;
}

Animation* AnimationManager::GetAnimation(const std::string& name) {
	// if the animation is not loaded, load it
	if(animations.find(name) == animations.end()) {
		LoadAnimation(name);
	}
	return &animations[name];
}

void AnimationManager::PreloadAnimation(const std::string& name) {
	Animation* animation = GetAnimation(name);
	animation->PreloadFrames();
}

void AnimationManager::PreloadAllAnimations() {
	// load all animations in the animations folder
	std::string path = "resources/animations/";
	std::vector<std::string> files_in_animation_dir = EngineUtils::GetFilesInDirectory(path);
	for(const std::string& file : files_in_animation_dir) {
		// get just the name of the file
		std::string name = file.substr(file.find_last_of("/\\") + 1);
		// remove the file extension
		name = name.substr(0, name.find_last_of("."));
		PreloadAnimation(name);
	}
}

void AnimationManager::UnloadAnimation(const std::string& name) {
	// if the animation is loaded, free it
	if(animations.find(name) != animations.end()) {
		animations[name].UnloadFrames();
		animations.erase(name);
	}
}