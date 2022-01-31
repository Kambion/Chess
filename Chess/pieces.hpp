#pragma once
#include <memory>

constexpr inline short sgn(int x) { return (x > 0) - (x < 0); }

enum class PieceColor { WHITE, BLACK };
enum class PieceType { PAWN, QUEEN, KING, ROOK, BISHOP, KNIGHT, WÊDZARNICZE };

inline PieceColor pieceColor(bool val) {
	return val ? PieceColor::BLACK : PieceColor::WHITE;
}

inline PieceColor operator~(const PieceColor& pc) {
	return pc == PieceColor::WHITE ? PieceColor::BLACK : PieceColor::WHITE;
}

class Piece {
protected:
	int x, y;
	bool hasMoved = false;
public:
	const PieceColor color;

	Piece(int x, int y, PieceColor color) : x(x), y(y), color(color) {}

	inline int getX() const { return x; }
	inline int getY() const { return y; }
	inline bool moved() const { return hasMoved; }

	void move(int xp, int yp);

	virtual int bmpOffset() const = 0;
	virtual bool checkMove(int xp, int yp) const = 0;
	virtual inline bool checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const { 
		return (!board[xp][yp]) || (board[xp][yp]->color != color); 
	}
	virtual inline bool checkPromote(int yp) { return false; }
	virtual inline void promote(std::unique_ptr<Piece> board[8][8], PieceType type) { return; }
	virtual inline PieceType type() const { return PieceType::WÊDZARNICZE; }
};

class Pawn : public Piece {
public:
	Pawn(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 5; }
	inline PieceType type() const override { return PieceType::PAWN; }
	bool checkMove(int xp, int yp) const override;
	bool checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const override;
	bool checkPromote(int yp) override;

	void promote(std::unique_ptr<Piece> board[8][8], PieceType type) override;
};

class Bishop : public Piece {
public:
	Bishop(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 2; }
	inline PieceType type() const override { return PieceType::BISHOP; }
	bool checkMove(int xp, int yp) const override;
	bool checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const override;
};

class Knight : public Piece {
public:
	Knight(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 3; }
	inline PieceType type() const override { return PieceType::KNIGHT; }
	bool checkMove(int xp, int yp) const override;
};

class King : public Piece {
public:
	King(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 0; }
	inline PieceType type() const override { return PieceType::KING; }
	bool checkMove(int xp, int yp) const override;
};

class Queen : public Piece {
public:
	Queen(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 1; }
	inline PieceType type() const override { return PieceType::QUEEN; }
	bool checkMove(int xp, int yp) const override;
	bool checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const override;
};

class Rook : public Piece {
public:
	Rook(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 4; }
	inline PieceType type() const override { return PieceType::ROOK; }
	bool checkMove(int xp, int yp) const override;
	bool checkCollision(int xp, int yp, std::unique_ptr<Piece> board[8][8]) const override;
};

class Wêdzarnicze : public Piece {
public:
	Wêdzarnicze(int x, int y, PieceColor color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 6; }
	inline PieceType type() const override { return PieceType::WÊDZARNICZE; }
	inline bool checkMove(int xp, int yp) const override { return abs(xp % (x + 2) - yp % (y + 1)) > 2; }
};