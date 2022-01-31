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
		board[i][1] = std::make_unique<Pawn>(i, 1, PieceColor::BLACK);
		board[i][6] = std::make_unique<Pawn>(i, 6, PieceColor::WHITE);
	}
	for (int i = 0; i < 8; i += 7) {
		board[0][i] = std::make_unique<Rook>(0, i,   pieceColor(!(i % 2)));
		board[7][i] = std::make_unique<Rook>(7, i,   pieceColor(!(i % 2)));
		board[1][i] = std::make_unique<Knight>(1, i, pieceColor(!(i % 2)));
		board[6][i] = std::make_unique<Knight>(6, i, pieceColor(!(i % 2)));
		board[2][i] = std::make_unique<Bishop>(2, i, pieceColor(!(i % 2)));
		board[5][i] = std::make_unique<Bishop>(5, i, pieceColor(!(i % 2)));
		board[3][i] = std::make_unique<Queen>(3, i,  pieceColor(!(i % 2)));
		board[4][i] = std::make_unique<King>(4, i,   pieceColor(!(i % 2)));
	}
}
void Game::initChoice(int x, int y, PieceColor color) {
	choice[0] = std::make_unique<Queen>(x, y, color);
	choice[1] = std::make_unique<Rook>(x, y, color);
	choice[2] = std::make_unique<Bishop>(x, y, color);
	choice[3] = std::make_unique<Knight>(x, y, color);
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
				window.drawPiece(j, i, piece->bmpOffset(), piece->color == PieceColor::BLACK);
			}
		}
	}
	window.update();
}

bool Game::movePiece(Piece *piece, int x, int y) {
	const int x0 = piece->getX(), y0 = piece->getY();
	if ((x != x0 || y != y0) && checkOnBoard(x, y)) {
		std::unique_ptr<Piece>& piece = board[x0][y0];
		if (piece && piece->checkMove(x, y) && piece->checkCollision(x, y, board)) {
			piece->move(x, y);
			if (piece->checkPromote(y)) {
				piece->promote(board, choiceLoop(x, y, piece->color));
				board[x0][y0] = nullptr;
			}
			else
				board[x][y] = std::move(piece);
			return true;
		}
	}
	return false;
}

void Game::event(int x, int y) {
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
			switch (state) {
			case State::GAME:
				mouseHandle(event.button.x, event.button.y);
				break;
			case State::PROMOTE:
				mouseHandleChoice(x, y, event.button.x, event.button.y);
				break;
			}
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
	Piece* p;
	if (checkOnBoard(x, y)) {
		if (selectedPiece && movePiece(selectedPiece, x, y)) {
			selectedPiece = nullptr;
			togglePlayer();
		}
		else if ((p = board[x][y].get()) && p->color == currentPlayer)
			selectedPiece = p;
		else
			selectedPiece = nullptr;
	}
	else
		selectedPiece = nullptr;
}

void Game::mouseHandleChoice(int x, int y, int mouseX, int mouseY) {
	const auto [cx, cy] = window.mapClick(mouseX, mouseY);
	if (cy == y && cx >= x - 1 && cx <= x + 2) {
		selectedPiece = choice[cx - x + 1].get();
	}
	else
		selectedPiece = nullptr;
}

void Game::highlightMoves(Piece* piece) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (piece->checkMove(j, i) && piece->checkCollision(j, i, board)) {
				window.highlightTile(j, i, Highlight::CIRCLE);
			}
		}
	}
}
PieceType Game::choiceLoop(int x, int y, PieceColor color) {
	state = State::PROMOTE;
	selectedPiece = nullptr;
	initChoice(x, y, color);
	while (state == State::PROMOTE) {
		event(x, y);
		drawChoice(x, y);
		if (selectedPiece) {
			state = State::GAME;
		}
	}
	return selectedPiece->type();
}
void Game::drawChoice(int x, int y) {
	window.drawChoice(x, y);
	for (int i = 0; i < 4; i++) {
		Piece* piece = choice[i].get();
		if (piece) {
			window.drawPiece(x-1+i, y, piece->bmpOffset(), piece->color == PieceColor::BLACK);
		}
	}
	window.update();
}