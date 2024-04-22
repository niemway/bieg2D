//
//  main.cpp
//  game_engine
//
//  Created by Nicholas Way on 1/25/24.
//

#include <stdio.h>
#include "Engine.hpp"

#undef main

int main(int argc, const char * argv[]) {
    Engine engine;
    engine.StartGameLoop();
    return 0;
}