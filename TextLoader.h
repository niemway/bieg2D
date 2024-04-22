//
//  TextLoader.h
//  game_engine
//
//  Created by Nicholas Way on 2/9/24.
//

#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>

//#ifdef __APPLE__
//#include <SDL2/SDL.h>
//#include <SDL2_ttf/SDL_ttf.h>
//#else
#include <SDL.h>
#include <SDL_ttf.h>
//#endif

#include "ImageLoader.h"

#pragma once

struct TextDrawCall {
	std::string text_content;
	std::string font;
	int size;
	int x;
	int y;
	SDL_Color font_color;

	TextDrawCall(std::string text_content, std::string font, int size, int x, int y, SDL_Color font_color) : text_content(text_content), font(font), size(size), x(x), y(y), font_color(font_color) {}
};

class TextLoader
{
public:
    
    static TextLoader& GetInstance(){
        static TextLoader instance;
        return instance;
    }
    
	static void LoadFont(std::string font_name, int size);
	static void ChangeFontSize(int size);
	// add a draw call to the draw_calls vector
	static void DrawText(std::string& text_content, std::string font_name, int size, int x, int y, SDL_Color font_color);
	static void ClearDrawCalls();
	static TTF_Font* GetCurrentFont();

	// functions accessible to lua
	static void Lua_DrawText(std::string text_content, int x, int y, std::string font_name, int font_size, int r, int g, int b, int a);

	// store a queue of text draw calls
	static inline std::queue<TextDrawCall> draw_calls;
	static inline std::vector<Image> rendered_text;

	static inline std::unordered_map<std::string, TTF_Font*> font_cache;

private:
	static TTF_Font* current_font;

};
