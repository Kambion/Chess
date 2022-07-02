#include "pieces.hpp"
#include <cmath>
#include <cstdlib>

bool Pawn::checkMove(int xp, int yp) const {
	return
		abs(x - xp) + abs(y - yp) <= 2 &&
		(y < yp) == (color == PieceColor::BLACK) &&
		y != yp;
}

bool Bishop::checkMove(int xp, int yp) const {
	return abs(x - xp) == abs(y - yp);
}

bool Knight::checkMove(int xp, int yp) const {
	return (abs(x - xp) + abs(y - yp) == 3) && x != xp && y != yp;
}

bool King::checkMove(int xp, int yp) const {
	return
		(abs(x - xp) <= 1 && abs(y - yp) <= 1) ||
		(!hasMoved && y == yp && abs(x - xp) == 2);
}

bool Queen::checkMove(int xp, int yp) const {
	return x == xp || y == yp || abs(x - xp) == abs(y - yp);
}

bool Rook::checkMove(int xp, int yp) const {
	return x == xp || y == yp;
}

void Piece::move(int xp, int yp) {
	if (type() == PieceType::PAWN && abs(y - yp) == 2)
		enPassant = true;
	x = xp;
	y = yp;
	hasMoved = true;
}

bool Pawn::checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const {
	if (!Piece::checkCollision(xp, yp, board))
		return false;
	const int dy = (color == PieceColor::BLACK ? 1 : -1);
	if (xp == x) {
		if (abs(y - yp) == 2) {
			if (hasMoved)
				return false;
			if (!board[x][yp] && !board[x][y + dy])
				return true;
		}
		else {
			if (!board[x][yp])
				return true;
		}
		return false;
	}
	if (board[xp][yp] && board[xp][yp]->color != color) {
		return true;
	}
	if (board[xp][y] && board[xp][y]->passant()) {
		return true;
	}
	return false;
}
bool Bishop::checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const {
	if (!Piece::checkCollision(xp, yp, board))
		return false;
	for (int i = x, j = y; i != xp; i += sgn(xp - x), j += sgn(yp-y)) {
		if ((i != x || j != y) && board[i][j]) {
			return false;
		}
	}
	return true;
}
bool Rook::checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const {
	if (!Piece::checkCollision(xp, yp, board))
		return false;
	const int di = sgn(xp - x), dj = sgn(yp - y);
	for (int i = x + di, j = y + dj; i != xp || j != yp; i += di, j += dj) {
		if (board[i][j]) {
			return false;
		}
	}
	return true;
}

bool Queen::checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const {
	if (!Piece::checkCollision(xp, yp, board))
		return false;
	const int di = sgn(xp - x), dj = sgn(yp - y);
	if (di == 0 || dj == 0) {
		for (int i = x + di, j = y + dj; i != xp || j != yp; i += di, j += dj) {
			if (board[i][j]) {
				return false;
			}
		}
		return true;
	}
	else {
		for (int i = x, j = y; i != xp; i += di, j += dj) {
			if ((i != x || j != y) && board[i][j]) {
				return false;
			}
		}
		return true;
	}
}
bool Pawn::checkPromote(int yp) {
	if ((color == PieceColor::BLACK && yp == 7) || (color == PieceColor::WHITE && yp == 0)) {
		return true;
	}
	return false;
}
void Pawn::promote(std::unique_ptr<Piece> board[8][8], PieceType type) {
	switch (type) {
	case PieceType::QUEEN:
		board[x][y] = std::make_unique<Queen>(x, y, color);
		break;
	case PieceType::KNIGHT:
		board[x][y] = std::make_unique<Knight>(x, y, color);
		break;
	case PieceType::ROOK:
		board[x][y] = std::make_unique<Rook>(x, y, color);
		break;
	case PieceType::BISHOP:
		board[x][y] = std::make_unique<Bishop>(x, y, color);
		break;
	default:
		return;
		break;
	}
}