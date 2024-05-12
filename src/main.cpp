#include "game.hpp"
#include "SDL3/SDL_main.h"

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	Game game{6000};
	game.run();
	return 0;
};
