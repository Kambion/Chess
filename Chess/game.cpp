#include "game.hpp"

void Game::Timer::tick() {
	t2 = SDL_GetTicks();
	delta = (1. * t2 - t1) * 0.001;
	t1 = t2;

	worldTime += delta;

	fpsTimer += delta;
	if (fpsTimer > 0.5) {
		fps = frames * 2.;
		frames = 0;
		fpsTimer -= 0.5;
	}
	frames++;
}

Game::Game(int time) : timeLeft({ time,time }) {
	initBoard();
}

void Game::run() {
	mainLoop();
}

void Game::mainLoop() {
	while (state == State::GAME) {
		event();
		draw();
	}
}

void Game::initBoard() {
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

void Game::draw() {
	window.drawBackground();
	window.drawBoard();
	if (selectedPiece) {
		int x = selectedPiece->getX();
		int y = selectedPiece->getY();
		window.highlightTile(x, y, Highlight::GREEN);
		highlightMoves(selectedPiece);
	}
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

void Game::movePiece(Piece *piece, int x, int y) {
	const int x0 = piece->getX(), y0 = piece->getY();
	if (checkOnBoard(x, y)) {
		std::unique_ptr<Piece>& piece = board[x0][y0];
		if (piece && piece->checkMove(x, y)) {
			piece->move(x, y);
			board[x][y] = std::move(piece);
		}
	}
}

void Game::event() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				state = State::EXIT;
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseHandle(event.button.x, event.button.y);
			break;
		case SDL_MOUSEBUTTONUP:
			break;
		case SDL_KEYUP:
			break;
		case SDL_QUIT:
			state = State::EXIT;
			break;
		};
	};
}

void Game::mouseHandle(int cx, int cy) {
	const auto [x, y] = window.mapClick(cx, cy);
	if (checkOnBoard(x, y)) {
		if (selectedPiece) {
				movePiece(selectedPiece, x, y);
				selectedPiece = nullptr;
		}
		else
			selectedPiece = board[x][y].get();
	}
	else
		selectedPiece = nullptr;
}
void Game::highlightMoves(Piece* piece) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (piece->checkMove(j, i)) {
				window.highlightTile(j, i, Highlight::CIRCLE);
			}
		}
	}
}