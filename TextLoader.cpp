#include "TextLoader.h"


TTF_Font* TextLoader::current_font = NULL;

TTF_Font* TextLoader::GetCurrentFont() { return current_font; }

void TextLoader::ClearDrawCalls() {
	// pop all draw calls from the draw_calls queue
	while (!draw_calls.empty()) {
		draw_calls.pop();
	}
}

void TextLoader::LoadFont(std::string font_name, int size) {

	TTF_Init();

	// check if the font is already in the font cache
	if (font_cache.find(font_name + std::to_string(size)) != font_cache.end()) {
		current_font = font_cache[font_name + std::to_string(size)];
		return;
	}

	// load the font from disk
	std::string path = "resources/fonts/" + font_name + ".ttf";
	current_font = TTF_OpenFont(path.c_str(), size);
	if (current_font == NULL) {
		std::cout << "error: font " << font_name << " missing";
		exit(0);
	}
	// add the font to the font cache
	font_cache[font_name + std::to_string(size)] = current_font;

}

void TextLoader::ChangeFontSize(int size) {
	TTF_SetFontSize(current_font, size);
}

void TextLoader::DrawText(std::string& text_content, std::string font_name, int size, int x, int y, SDL_Color font_color) {
	// add a draw call to the draw_calls queue
	draw_calls.emplace(text_content, font_name, size, x, y, font_color);
}

void TextLoader::Lua_DrawText(std::string text_content, int x, int y, std::string font_name, int font_size, int r, int g, int b, int a) {
	SDL_Color color = { (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a };
	// add a draw call to the draw_calls queue
	draw_calls.emplace(text_content, font_name, font_size, x, y, color);
}