#pragma once
#include <memory>

class Piece {
protected:
	int x, y;
	bool hasMoved = false;
public:
	const bool color;
	
	Piece(int x, int y, bool color) : x(x), y(y), color(color) {}

	inline int getX() const { return x; }
	inline int getY() const { return y; }
	inline bool moved() const { return hasMoved; }

	void move(int xp, int yp);

	virtual int bmpOffset() const = 0;
	virtual bool checkMove(int xp, int yp) const = 0;
};

class Pawn : public Piece {
public:
	Pawn(int x, int y, bool color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 5; }
	bool checkMove(int xp, int yp) const override;
};

class Bishop : public Piece {
public:
	Bishop(int x, int y, bool color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 2; }
	bool checkMove(int xp, int yp) const override;
};

class Knight : public Piece {
public:
	Knight(int x, int y, bool color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 3; }
	bool checkMove(int xp, int yp) const override;
};

class King : public Piece {
public:
	King(int x, int y, bool color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 0; }
	bool checkMove(int xp, int yp) const override;
};

class Queen : public Piece {
public:
	Queen(int x, int y, bool color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 1; }
	bool checkMove(int xp, int yp) const override;
};

class Rook : public Piece {
public:
	Rook(int x, int y, bool color) : Piece(x, y, color) {}

	inline int bmpOffset() const override { return 4; }
	bool checkMove(int xp, int yp) const override;
};

class W�dzarnicze : public Piece {};