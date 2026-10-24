//
//  main.cpp
//  game_engine
//
//  Created by Nicholas Way on 1/25/24.
//

#include <stdio.h>
#include "Engine.hpp"

#ifdef __APPLE__
#include <unistd.h>
#include <string>
#endif

#undef main

int main(int argc, const char * argv[]) {
#ifdef __APPLE__
    // when launched as a .app, the working directory is "/", so move into
    // Contents/Resources where the bundled resources/ folder lives
    if (char* base_path = SDL_GetBasePath()) {
        if (std::string(base_path).find(".app/Contents/Resources") != std::string::npos)
            chdir(base_path);
        SDL_free(base_path);
    }
#endif

    Engine engine;
    engine.StartGameLoop();
    return 0;
}