//
//  Renderer.cpp
//  game_engine
//
//  Created by Nicholas Way on 2/6/24.
//

#include "Renderer.hpp"
#include "Helper.h"
//#include <SDL2/SDL.h>
#include <iostream>
#include <SDL.h>

void Renderer::init(std::string game_title, int window_height, int window_width, uint8_t *clear_color, float zoom_factor){
	CreateWindow(game_title, window_height, window_width);
	// Create the renderer
	renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	camera_dimensions = glm::ivec2(window_width, window_height);
    cam_zoom_factor = zoom_factor;
	screen_clear_color = { clear_color[0], clear_color[1], clear_color[2], clear_color[3] };

	SDL_SetRenderDrawColor(renderer, screen_clear_color.r, screen_clear_color.g, screen_clear_color.b, 255);
	SDL_RenderClear(renderer);
}

void Renderer::CreateWindow(std::string game_title, int window_height, int window_width){
	// Initialize the window
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	// Create the window
    window = Helper::SDL_CreateWindow498(game_title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		window_width, window_height,
		SDL_WINDOW_SHOWN);
	
	if(window == NULL){
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
}

void Renderer::RenderImage(SDL_Texture* texture, int x, int y, int w, int h){
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Renderer::RenderAllImages(){
	std::stable_sort(image_draw_calls.begin(), image_draw_calls.end(), [](ImageDrawCall a, ImageDrawCall b) {
		return a.sorting_order < b.sorting_order;
	});
	SDL_RenderSetScale(renderer, cam_zoom_factor, cam_zoom_factor);
	
	for(auto& draw_call : image_draw_calls){
		glm::vec2 final_rendering_position = glm::vec2(draw_call.x, draw_call.y) - camera_center;
		SDL_Texture* texture = ImageLoader::GetInstance().LoadImage(draw_call.image_name);
		SDL_Rect rect;
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

		int flip = SDL_FLIP_NONE;
		if (draw_call.scale_x < 0) {
			flip |= SDL_FLIP_HORIZONTAL;
		}
		if (draw_call.scale_y < 0) {
			flip |= SDL_FLIP_VERTICAL;
		}
		float x_scale_abs = std::abs(draw_call.scale_x);
		float y_scale_abs = std::abs(draw_call.scale_y);
		rect.w *= x_scale_abs;
		rect.h *= y_scale_abs;

		SDL_Point pivot = { static_cast<int>(rect.w * draw_call.pivot_x), static_cast<int>(rect.h * draw_call.pivot_y) };

		rect.x = static_cast<int>(final_rendering_position.x * pixels_per_unit + camera_dimensions.x * 0.5f * (1.0f / cam_zoom_factor) - pivot.x);
		rect.y = static_cast<int>(final_rendering_position.y * pixels_per_unit + camera_dimensions.y * 0.5f * (1.0f / cam_zoom_factor) - pivot.y);

		// apply tint/ alpha to texture
		SDL_SetTextureColorMod(texture, draw_call.r, draw_call.g, draw_call.b);
		SDL_SetTextureAlphaMod(texture, draw_call.a);

		Helper::SDL_RenderCopyEx498(0, "nicholas", renderer, texture, NULL, &rect, draw_call.rotation_degrees, &pivot, static_cast<SDL_RendererFlip>(flip));

		// SDL_RenderSetScale(renderer, cam_zoom_factor, cam_zoom_factor);

		// reset tint/ alpha
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);

	} // for (draw_call : image_draw_calls)

	SDL_RenderSetScale(renderer, 1, 1);
	image_draw_calls.clear();
}

void Renderer::RenderAllUI(){
	std::stable_sort(ui_draw_calls.begin(), ui_draw_calls.end(), [](UIDrawCall a, UIDrawCall b) {
		return a.sorting_order < b.sorting_order;
	});
	for(auto& draw_call : ui_draw_calls){
		SDL_Texture* texture = ImageLoader::GetInstance().LoadImage(draw_call.image_name);
		SDL_Rect rect;
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
		rect.x = draw_call.x;
		rect.y = draw_call.y;
		SDL_SetTextureColorMod(texture, draw_call.r, draw_call.g, draw_call.b);
		SDL_SetTextureAlphaMod(texture, draw_call.a);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
	ui_draw_calls.clear();
}

void Renderer::RenderAllText(){
	// get the font
	TTF_Font* font;
	// get next draw call from the draw_calls queue
	while(!TextLoader::GetInstance().draw_calls.empty()){
		// get the next draw call
		TextDrawCall draw_call = TextLoader::GetInstance().draw_calls.front();
		TextLoader::GetInstance().draw_calls.pop();

		TextLoader::GetInstance().LoadFont(draw_call.font, draw_call.size);
		font = TextLoader::GetInstance().GetCurrentFont();
		SDL_Texture* texture = NULL;
		// check if the text has already been rendered
		for (auto& text : TextLoader::GetInstance().rendered_text) {
			if (text.name == draw_call.text_content) {
				// if the texture has already been generated, use it
				texture = text.texture;
                break;
			}
		}
		// if the texture has not been generated, create it
		if (texture == NULL) {
			SDL_Surface* surface = TTF_RenderText_Solid(font, draw_call.text_content.c_str(), draw_call.font_color);
			if (surface == NULL) {
				std::cout << "Error rendering text: " << SDL_GetError() << std::endl;
				exit(0);
			}
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			TextLoader::GetInstance().rendered_text.push_back({ draw_call.text_content, texture });
		}
		SDL_Rect rect;
		rect.x = draw_call.x;
		rect.y = draw_call.y;
		int w, h;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		rect.w = w;
		rect.h = h;
		TextLoader::GetInstance().ChangeFontSize(draw_call.size);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
	}
}

void Renderer::RenderAllPixelCalls(){
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for(auto& draw_call : pixel_draw_calls){
		SDL_SetRenderDrawColor(renderer, draw_call.r, draw_call.g, draw_call.b, draw_call.a);
		SDL_RenderDrawPoint(renderer, draw_call.x, draw_call.y);
	}
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	pixel_draw_calls.clear();
}

glm::vec2 Renderer::GetCameraCenter(){
	return camera_center;
}

glm::ivec2 Renderer::GetCameraDimensions(){
	return camera_dimensions;
}

float Renderer::GetCameraZoomFactor(){
	return cam_zoom_factor;
}

void Renderer::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	screen_clear_color = { r, g, b, a };
}

void Renderer::ClearScreen(){
    SDL_SetRenderDrawColor(renderer, screen_clear_color.r, screen_clear_color.g, screen_clear_color.b, 255);
    SDL_RenderClear(renderer);
}

// ----------------- functions accessible to lua -----------------

void Renderer::Lua_DrawUI(std::string image_name, float x, float y){
	ui_draw_calls.emplace_back(image_name, static_cast<int>(x), static_cast<int>(y), 255, 255, 255, 255, 0);
}

void Renderer::Lua_DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order){
	ui_draw_calls.emplace_back(image_name, static_cast<int>(x), static_cast<int>(y), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), sorting_order);
}

void Renderer::Lua_DrawImage(std::string image_name, float x, float y){
	image_draw_calls.emplace_back(image_name, x, y, 0, 1, 1, 0.5f, 0.5f, 255, 255, 255, 255, 0);
}

void Renderer::Lua_DrawImageEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order){
	image_draw_calls.emplace_back(image_name, x, y, static_cast<int>(rotation_degrees), scale_x, scale_y, pivot_x, pivot_y, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), sorting_order);
}

void Renderer::Lua_DrawPixel(float x, float y, float r, float g, float b, float a){
	pixel_draw_calls.emplace_back(static_cast<int>(x), static_cast<int>(y), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
}

void Renderer::Lua_SetCameraCenter(float x, float y){
	camera_center = glm::vec2(x, y);
}

float Renderer::Lua_GetCameraCenterX(){
	return camera_center.x;
}

float Renderer::Lua_GetCameraCenterY(){
	return camera_center.y;
}

void Renderer::Lua_SetCameraZoom(float zoom_factor){
	cam_zoom_factor = zoom_factor;
}

float Renderer::Lua_GetCameraZoom(){
	return cam_zoom_factor;
}
