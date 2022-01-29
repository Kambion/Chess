#pragma once
#include <memory>
#include "pieces.hpp"
#include "SDLgraphics.hpp"

class Game {
private:
	struct Timer {
		int t1, t2 = 0, frames = 0, lastState = 0;
		double delta = 0, worldTime = 0, fpsTimer = 0, fps = 0;

		Timer() : t1(SDL_GetTicks()) {}

		void tick();
	};
	enum class State { MENU, GAME, ENDG, EXIT };

	static constexpr std::string_view TITLE = "CHESS BY KAMBION GAMEDEV COMPANY";
	static constexpr int SCREEN_WIDTH = 1080;
	static constexpr int SCREEN_HEIGHT = 720;

	std::unique_ptr<Piece> board[8][8];
	Piece* selectedPiece{ nullptr };
	Timer timer{};
	State state = State::GAME;
	struct { int white, black; } timeLeft;
	SDLWindow window{SCREEN_WIDTH, SCREEN_HEIGHT, TITLE};
public:
	Game(int time);

	void run();
private:
	inline constexpr bool checkOnBoard(int x, int y) const { return x >= 0 && y >= 0 && x < 8 && y < 8; }

	void movePiece(Piece *piece, int x, int y);
	void event();
	void mouseHandle(int x, int y);
	void mainLoop();
	void initBoard();
	void draw();
	void highlightMoves(Piece* piece);
};
