# Nicholas Way game engine Makefile

LDLIBS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.4
LDFLAGS := -L./SDL_Frameworks/
INCLUDES := -I/glm -I./rapidjson-1.1.0/include/ -I./SDL_Frameworks/VS_Includes/include/ -I./lua -I./LuaBridge -I./box2d-2.4.1/include/ -I./box2d-2.4.1/src

EXE_NAME = game_engine_linux

BOX2D_SRC = $(wildcard ./box2d-2.4.1/src/**/*.cpp)
BOX2D_OBJ = $(BOX2D_SRC:.cpp=.o)

game_engine_linux: $(BOX2D_OBJ)
	clang++ *.cpp $(BOX2D_OBJ) -O3 -std=c++17 $(INCLUDES) $(LDFLAGS) $(LDLIBS) -o$(EXE_NAME)

%.o: %.cpp
	clang++ -c $< -O3 -std=c++17 $(INCLUDES) -o $@

clean:
	rm -f *.o game_engine_linux

