//
//  TextLoader.h
//  game_engine
//
//  Created by Nicholas Way on 2/9/24.
//

#pragma once

#include <string>
#include <vector>
#include <iostream>

//#ifdef __APPLE__
//#include <SDL2_mixer/SDL_mixer.h>
//#else
#include <SDL_mixer.h>
//#endif

#include "AudioHelper.h"

struct Audio {
	std::string name;
	Mix_Chunk* chunk;

	Audio(std::string name, Mix_Chunk* chunk) : name(name), chunk(chunk) {}

	// copy constructor
	Audio(const Audio& audio) {
		name = audio.name;
		chunk = audio.chunk;
	}
};

class AudioLoader
{
public:

	// make singleton
	static AudioLoader& GetInstance() {
		static AudioLoader instance;
		return instance;
	}

	static void Init();
	static std::shared_ptr<Audio> LoadAudio(std::string audio_name);
	static std::shared_ptr<Audio> GetAudio(std::string audio_name);
	static void PlayAudio(std::string audio_name, bool looping, int channel);
	static void PlayAudio(Audio* audio, bool looping, int channel);

	// functions accessible to lua
	static void Lua_PlayAudio(int channel, std::string clip_name, bool does_loop);
	static void Lua_HaltAudio(int channel);
	static void Lua_SetChannelVolume(int channel, int volume);

	// store a map of audio
	static inline std::unordered_map<std::string, std::shared_ptr<Audio>> all_audio;
};

