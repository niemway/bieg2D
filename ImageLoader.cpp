#include "ImageLoader.h"

SDL_Texture* ImageLoader::LoadImage(const std::string& name, SDL_Renderer* renderer) {
	// if the image is already loaded in the map, return it
	if(images.find(name) != images.end()) {
		return images[name].texture;
	}

	// otherwise, load the image from disk and store it in the vector
	std::string path = "resources/images/" + name + ".png";
	SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
	if (texture == NULL) {
//		std::cout << SDL_GetError() << std::endl;
		std::cout << "error: missing image " << name;
		exit(0);
	}
	Image image;
	image.name = name;
	image.texture = texture;
	images[name] = image;
	return texture;
}

SDL_Texture* ImageLoader::LoadImage(const std::string& name) {
	return LoadImage(name, renderer);
}

void ImageLoader::Init(SDL_Renderer* renderer) {
	ImageLoader::renderer = renderer;
}

void ImageLoader::PreloadImage(const std::string& name) {
	LoadImage(name);
}

void ImageLoader::PreloadAllImages() {
	// load all images in the images folder
	std::string path = "resources/images/";
	std::vector<std::string> files_in_image_dir = EngineUtils::GetFilesInDirectory(path);
	for(const std::string& file : files_in_image_dir) {
		// get just the name of the file
		std::string name = file.substr(file.find_last_of("/\\") + 1);
		// remove the file extension
		name = name.substr(0, name.find_last_of("."));
		PreloadImage(name);
	}
}

void ImageLoader::UnloadImage(const std::string& name) {
	// if the image is loaded, free it
	if(images.find(name) != images.end()) {
		SDL_DestroyTexture(images[name].texture);
		images.erase(name);
	}
}
