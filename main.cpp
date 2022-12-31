#define _USE_MATH_DEFINES
#include"game.h"

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	
	//game class object
	Game game;

	//calling object methods
	game.setup();
	game.loadImages();
	game.mainLoop();
	game.clearVideo();

	return 0;
};
