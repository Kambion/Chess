#include "game.hpp"

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	Game game{6000};
	game.run();
	return 0;
};
