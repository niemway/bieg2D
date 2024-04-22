#include "AudioLoader.h"

void AudioLoader::Init() {
	// initialize the audio mixer
	if (AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cerr << "error: audio mixer failed to initialize";
		exit(0);
	}
	AudioHelper::Mix_AllocateChannels498(50);
}

std::shared_ptr<Audio> AudioLoader::LoadAudio(std::string audio_name) {
	// if the audio is already loaded, return the audio
	if (all_audio.find(audio_name) != all_audio.end()) {
		return all_audio[audio_name];
	}
	// otherwise, load the audio from disk and store it in the vector
	std::string path = "resources/audio/" + audio_name + ".wav";
	Mix_Chunk* chunk = AudioHelper::Mix_LoadWAV498(path.c_str());
	if (chunk == NULL) {
		path = "resources/audio/" + audio_name + ".ogg";
		chunk = AudioHelper::Mix_LoadWAV498(path.c_str());
	}
	if (chunk == NULL) {
        std::cout << SDL_GetError() << std::endl;
		std::cout << "error: failed to play audio clip " << audio_name;
		exit(0);
	}
	
	Audio audio = Audio(audio_name, chunk);
	all_audio[audio_name] = std::make_shared<Audio>(audio);
	return all_audio[audio_name];
}

std::shared_ptr<Audio> AudioLoader::GetAudio(std::string audio_name) {
	// find the audio in the audio_cache map and return it
	if(all_audio.find(audio_name) != all_audio.end())
		return all_audio[audio_name];

	// if the audio is not found, load it and return it
	return LoadAudio(audio_name);
}

void AudioLoader::PlayAudio(std::string audio_name, bool looping, int channel) {
	// find the audio in the audio_cache map and play it
	if(all_audio.find(audio_name) != all_audio.end()){
		PlayAudio(all_audio[audio_name].get(), looping, channel);
		return;
	}
	// if the audio is not found, load it and play it
	LoadAudio(audio_name);
	PlayAudio(audio_name, looping, channel);
}

void AudioLoader::PlayAudio(Audio* audio, bool looping, int channel) {
    if(audio != nullptr)
        AudioHelper::Mix_PlayChannel498(channel, audio->chunk, looping ? -1 : 0);
}

// -----------  functions accessible to lua  ------------

void AudioLoader::Lua_PlayAudio(int channel, std::string clip_name, bool does_loop) {
	PlayAudio(clip_name, does_loop, channel);
}

void AudioLoader::Lua_HaltAudio(int channel) {
	AudioHelper::Mix_HaltChannel498(channel);
}

void AudioLoader::Lua_SetChannelVolume(int channel, int volume) {
	AudioHelper::Mix_Volume498(channel, volume);
}
