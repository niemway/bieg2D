//
//  Renderer.hpp
//  game_engine
//
//  Created by Nicholas Way on 2/6/24.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <stdio.h>
#include <SDL.h>
#include <string>

#include "glm/glm.hpp"
#include "TextLoader.h"
#include "Actor.hpp"

struct ImageDrawCall{
    std::string image_name;
    float x;
    float y;
    int rotation_degrees;
    float scale_x;
    float scale_y;
    float pivot_x;
    float pivot_y;
    int r;
    int g;
    int b;
    int a;
    int sorting_order;

    ImageDrawCall(std::string image_name, float x, float y, int rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, int r, int g, int b, int a, int sorting_order) : image_name(image_name), x(x), y(y), rotation_degrees(rotation_degrees), scale_x(scale_x), scale_y(scale_y), pivot_x(pivot_x), pivot_y(pivot_y), r(r), g(g), b(b), a(a), sorting_order(sorting_order) {}
};

struct UIDrawCall{
    std::string image_name;
    int x;
    int y;
    int r;
    int g;
    int b;
    int a;
    int sorting_order;

    UIDrawCall(std::string image_name, int x, int y, int r, int g, int b, int a, int sorting_order) : image_name(image_name), x(x), y(y), r(r), g(g), b(b), a(a), sorting_order(sorting_order) {}
};

struct PixelDrawCall{
    int x;
    int y;
    int r;
    int g;
    int b;
    int a;

    PixelDrawCall(int x, int y, int r, int g, int b, int a) : x(x), y(y), r(r), g(g), b(b), a(a) {}
};

class Renderer{
    
public:
    void init(std::string game_title, int window_height, int window_width, uint8_t *clear_color, float zoom_factor);
    void CreateWindow(std::string game_title, int window_height, int window_width);
    void RenderImage(SDL_Texture* texture, int x, int y, int w, int h);
    void RenderActor(Actor* actor, glm::vec2 camera_center, glm::vec2 camera_size, float zoom_factor);
    void RenderAllImages();
    void RenderAllUI();
    void RenderAllText();
    void RenderAllPixelCalls();

    glm::vec2 GetCameraCenter();
    glm::ivec2 GetCameraDimensions();
    float GetCameraZoomFactor();

    void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void ClearScreen();

    // functions accessible to lua
    static void Lua_DrawUI(std::string image_name, float x, float y);
    static void Lua_DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order);
    static void Lua_DrawImage(std::string image_name, float x, float y);
    static void Lua_DrawImageEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order);
    static void Lua_DrawPixel(float x, float y, float r, float g, float b, float a);

    static void Lua_SetCameraCenter(float x, float y);
    static float Lua_GetCameraCenterX();
    static float Lua_GetCameraCenterY();
    static void Lua_SetCameraZoom(float zoom_factor);
    static float Lua_GetCameraZoom();

    SDL_Renderer* renderer;

private:
    SDL_Window* window;

    static inline float cam_zoom_factor = 1.0f;

    int pixels_per_unit = 100;

    SDL_Color screen_clear_color;

    static inline glm::vec2 camera_center = glm::vec2(0, 0);
    static inline glm::ivec2 camera_dimensions = glm::ivec2(640, 360);

    static inline std::vector<ImageDrawCall> image_draw_calls;
    static inline std::vector<UIDrawCall> ui_draw_calls;
    static inline std::vector<PixelDrawCall> pixel_draw_calls;
};

#endif /* Renderer_hpp */
