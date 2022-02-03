#pragma once
#include <memory>
#include "pieces.hpp"
#include "SDLgraphics.hpp"

class Game {
private:
	struct KingPos {
		int x = 0, y = 0;
	};
	struct Timer {
		int t1, t2 = 0, frames = 0, lastState = 0;
		double delta = 0, worldTime = 0, fpsTimer = 0, fps = 0;

		Timer() : t1(SDL_GetTicks()) {}

		void tick();
	};
	enum class State { MENU, GAME, ENDG, EXIT, PROMOTE };

	static constexpr std::string_view TITLE = "CHESS BY KAMBION GAMEDEV COMPANY";
	static constexpr int SCREEN_WIDTH = 1080;
	static constexpr int SCREEN_HEIGHT = 720;

	std::unique_ptr<Piece> board[8][8];
	std::unique_ptr<Piece> choice[4];
	Piece* selectedPiece{ nullptr };
	Timer timer{};
	State state = State::GAME;
	PieceColor currentPlayer = PieceColor::WHITE;
	struct { int white, black; } timeLeft;
	SDLWindow window{SCREEN_WIDTH, SCREEN_HEIGHT, TITLE};
public:
	Game(int time);

	void run();
private:
	inline constexpr bool checkOnBoard(int x, int y) const { return x >= 0 && y >= 0 && x < 8 && y < 8; }
	inline void togglePlayer() { currentPlayer = ~currentPlayer; }

	KingPos kingPos(PieceColor color, std::unique_ptr<Piece> boardToCheck[8][8]);
	bool movePiece(Piece *piece, int x, int y);
	void event(int x = NULL, int y = NULL);
	void resetEnPassant();
	void mouseHandle(int x, int y);
	void mouseHandleChoice(int x, int y, int mouseX, int mouseY);
	PieceType choiceLoop(int x, int y, PieceColor color);
	void mainLoop();
	void endGameLoop();
	void initBoard();
	void initChoice(int x, int y, PieceColor color);
	void draw();
	void drawChoice(int x, int y);
	int highlightMoves(Piece* piece, bool highlight = true);
	bool checkCheck(PieceColor color);
	bool checkNextCheck(PieceColor color, int x, int y, int x0, int y0);
	void copyBoard(std::unique_ptr<Piece> boardCopy[8][8]);
	bool checkPossibleMoves(PieceColor color);
	void checkEndGame();
};
