#include "pieces.hpp"
#include <cmath>

bool Pawn::checkMove(int xp, int yp) const {
	return (abs(x - xp) + abs(y - yp) <= 2) && ((y < yp) == color) && (y != yp);
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
	x = xp;
	y = yp;
	hasMoved = true;
}