//
//  ImageLoader.h
//  game_engine
//
//  Created by Nicholas Way on 2/9/24.
//

#ifndef ImageLoader_hpp
#define ImageLoader_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "EngineUtils.h"

//#ifdef __APPLE__
//#include <SDL2_image/SDL_image.h>
//#else
#include <SDL_image.h>
//#endif

// struct to store the image and its name
struct Image {
	std::string name;
	SDL_Texture* texture;
};

class ImageLoader {

public:

	// make singleton
	static ImageLoader& GetInstance() {
		static ImageLoader instance;
		return instance;
	}

	static void Init(SDL_Renderer *renderer);
	// Load an image from a file
	static SDL_Texture* LoadImage(const std::string &name, SDL_Renderer *renderer);
	static SDL_Texture* LoadImage(const std::string &name);

	static void PreloadImage(const std::string &name);
	static void PreloadAllImages();
	static void UnloadImage(const std::string &name);

	ImageLoader() = default;
	~ImageLoader() = default;
	ImageLoader(const ImageLoader&) = delete;
	ImageLoader& operator=(const ImageLoader&) = delete;

private:

	// map to store all the images
	static inline std::map<std::string, Image> images;
	static inline SDL_Renderer *renderer;

};

#endif /* ImageLoader_hpp */
