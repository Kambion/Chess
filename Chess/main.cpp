#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include <time.h>
#include <cstdlib>
#include <memory>
#include "SDLgraphics.hpp"
#include "pieces.hpp"


extern "C" {
#include "SDL.h"
#include "SDL_main.h"
}

#define TITLE "CHESS BY KAMBION GAMEDEV COMPANY"
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

struct timer_t {
	int t1, t2, frames, lastState;
	double delta, worldTime, fpsTimer, fps;
};

void initTimer(timer_t& timer) {
	timer.t1 = SDL_GetTicks();
	timer.frames = 0;
	timer.fpsTimer = 0;
	timer.fps = 0;
	timer.worldTime = 0;
}

void timerFunction(timer_t& timer) {
	timer.t2 = SDL_GetTicks();
	timer.delta = (timer.t2 - timer.t1) * 0.001;
	timer.t1 = timer.t2;

	timer.worldTime += timer.delta;

	timer.fpsTimer += timer.delta;
	if (timer.fpsTimer > 0.5) {
		timer.fps = timer.frames * 2;
		timer.frames = 0;
		timer.fpsTimer -= 0.5;
	};
}
void initBoard(std::unique_ptr<Piece> board[8][8]) {
	for (int i = 0; i < 8; i++) {
		board[i][1] = std::make_unique<Pawn>(i, 1, true);
		board[i][6] = std::make_unique<Pawn>(i, 6, false);
	}
	for (int i = 0; i < 8; i += 7) {
		board[0][i] = std::make_unique<Rook>(0, i, !(i % 2));
		board[7][i] = std::make_unique<Rook>(7, i, !(i % 2));
		board[1][i] = std::make_unique<Knight>(1, i, !(i % 2));
		board[6][i] = std::make_unique<Knight>(6, i, !(i % 2));
		board[2][i] = std::make_unique<Bishop>(2, i, !(i % 2));
		board[5][i] = std::make_unique<Bishop>(5, i, !(i % 2));
		board[3][i] = std::make_unique<Queen>(3, i, !(i % 2));
		board[4][i] = std::make_unique<King>(4, i, !(i % 2));
	}
}
void gameMain(SDLWindow &window) {
	SDL_Event event;
	bool quit = false;
	std::unique_ptr<Piece> board[8][8]{};
	initBoard(board);
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				quit = true;
				break;
			};
		};
		window.drawBackground();
		window.drawBoard();
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				Piece* piece = board[j][i].get();
				if (piece) {
					window.drawPiece(j, i, piece->bmpOffset(), piece->color);
				}
			}
		}
		window.update();
	}
}
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	SDLWindow window{ SCREEN_WIDTH, SCREEN_HEIGHT, TITLE };
	gameMain(window);
	return 0;
};
