#include "game.hpp"
#include <iostream>

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
	endGameLoop();
}

void Game::mainLoop() {
	while (state == State::GAME) {
		event();
		draw();
		checkEndGame();
	}
}

void Game::endGameLoop() {
	while (state == State::ENDG) {
		draw();
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
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				state = State::EXIT;
				break;
			};
		};
	}
}

void Game::checkEndGame() {
	if (!checkPossibleMoves(currentPlayer)) {
		if (checkCheck(currentPlayer)) {
			switch (currentPlayer) {
			case PieceColor::WHITE:
				std::cout << "Bialy przejebal";
				break;
			case PieceColor::BLACK:
				std::cout << "Czarny przejebal";
				break;
			}
		}
		else {
			std::cout << "PAT";
		}
		state = State::ENDG;
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
		if (piece->type() == PieceType::KING && abs(x - x0) == 2 && y == y0) {
			if (!checkCastle(piece->color, x, y)) {
				return false;
			}
		}
		if (piece && piece->checkMove(x, y) && piece->checkCollision(x, y, board) && !checkNextCheck(currentPlayer, x, y, x0, y0)) {
			if (piece->type() == PieceType::PAWN && x != x0) {
				if (board[x][y0] && board[x][y0]->passant()) {
					board[x][y0] = nullptr;
				}
			}
			if (piece->type() == PieceType::KING && abs(x - x0) == 2 && y == y0) {
				int dx = (x - x0) / 2;
				if (dx > 0) {
					if (board[7][y]) {
						board[(x + x0) / 2][y] = std::move(board[7][y]);
					}
				}
				else {
					if (board[0][y]) {
						board[(x + x0) / 2][y] = std::move(board[0][y]);
					}
				}
			}
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
			resetEnPassant();
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

int Game::highlightMoves(Piece* piece, bool highlight) {
	int possibleMoves = 0;
	bool castle = false;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (piece->type() == PieceType::KING && abs(j - piece->getX()) == 2 && piece->getY() == i) {
				castle = true;
				if (checkCastle(piece->color, j, piece->getY())) {
					if (highlight)
						window.highlightTile(j, i, Highlight::CIRCLE);
					possibleMoves++;
				}
			}
			if (piece->checkMove(j, i) && piece->checkCollision(j, i, board) && !checkNextCheck(currentPlayer, j, i, piece->getX(), piece->getY()) && !castle) {
				if(highlight)
					window.highlightTile(j, i, Highlight::CIRCLE);
				possibleMoves++;
			}
			castle = false;
		}
	}
	return possibleMoves;
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
void Game::resetEnPassant() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[j][i] && board[j][i]->color == currentPlayer) {
				board[j][i]->resetPassant();
			}
		}
	}
}
Game::KingPos Game::kingPos(PieceColor color, std::unique_ptr<Piece> boardToCheck[8][8]) {
	KingPos pos;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (boardToCheck[j][i] && boardToCheck[j][i]->type() == PieceType::KING && boardToCheck[j][i]->color == color) {
				pos.x = j;
				pos.y = i;
			}
		}
	}
	return pos;
}
bool Game::checkCheck(PieceColor color) {
	KingPos pos = kingPos(color, board);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[j][i] && board[j][i]->checkMove(pos.x, pos.y) && board[j][i]->checkCollision(pos.x, pos.y, board)) {
				return true;
			}
		}
	}
	return false;
}
bool Game::checkNextCheck(PieceColor color, int x, int y, int x0, int y0) {
	std::unique_ptr<Piece> boardCopy[8][8];
	copyBoard(boardCopy);
	if (boardCopy[x0][y0] && boardCopy[x0][y0]->type() == PieceType::PAWN && x != x0) {
		if (boardCopy[x][y0] && boardCopy[x][y0]->passant()) {
			boardCopy[x][y0] = nullptr;
		}
	}
	boardCopy[x][y] = std::move(boardCopy[x0][y0]);
	KingPos pos = kingPos(color, boardCopy);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (boardCopy[j][i] && boardCopy[j][i]->checkMove(pos.x, pos.y) && boardCopy[j][i]->checkCollision(pos.x, pos.y, boardCopy)) {
				return true;
			}
		}
	}
	return false;
}
void Game::copyBoard(std::unique_ptr<Piece> boardCopy[8][8]) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[j][i]) {
				boardCopy[j][i] = board[j][i]->copy();
			}
		}
	}
}
bool Game::checkPossibleMoves(PieceColor color) {
	int possibleMoves = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[j][i] && board[j][i]->color == color) {
				possibleMoves += highlightMoves(board[j][i].get(), false);
			}
		}
	}
	return possibleMoves;
}
bool Game::checkCastle(PieceColor color, int x, int y) {
	KingPos pos = kingPos(color, board);
	if (checkCheck(color) || board[pos.x][pos.y]->moved())
		return false;
	int dx = (x - pos.x)/2;
	int tiles = 0;
	if (dx > 0) {
		if (!board[7][y] || board[7][y]->type() != PieceType::ROOK || board[7][y]->moved())
			return false;
		tiles = 2;
	}
	else {
		if (!board[0][y] || board[0][y]->type() != PieceType::ROOK || board[0][y]->moved())
			return false;
		tiles = 3;
	}
	for (int i = 1; i <= tiles; i++) {
		if (board[pos.x + i * dx][y]) {
			return false;
		}
	}
	if (checkNextCheck(color, (x + pos.x) / 2, y, pos.x, pos.y) || checkNextCheck(color, x, y, pos.x, pos.y))
		return false;
	return true;
}