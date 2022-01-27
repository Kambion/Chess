#pragma once

extern "C" {
#include "SDL.h"
#include "SDL_main.h"
}

#include <string_view>

class SDLWindow {
private:	//niezmienne dla user(debil)
	static constexpr int pieceSize = 70;
	static constexpr int tileSize = 70;
	static constexpr int boardSize = 8;
	static constexpr char
		backgroundName[] = "background.bmp",
		charsetName[] = "cs8x8.bmp",
		piecesName[] = "pieces.bmp";

	int w, h;
	std::string_view title;

	SDL_Surface* screen;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	struct {
		SDL_Surface* charset, * pieces, * background,
		*deskaRustykalna03NaŒcianêJasnyBr¹z;
	} images;
	struct {
		Uint32 white, light_gary, dark_gary;
	} colors;
public: //user(debil) mo¿e popsuæ
	SDLWindow(int width, int height, std::string_view title);

	inline int width() const { return w; } 
	inline int height() const { return h; }
	inline std::string_view windowTitle() const { return title; }

	Uint32 mapColor(int rgb) const;

	void drawPixel(int x, int y, Uint32 color);
	void drawLine(int x, int y, int l, int dx, int dy, Uint32 color);
	void drawRectangle(SDL_Rect rect, Uint32 fillColor);
	void drawRectangle(SDL_Rect rect, int thickness, Uint32 outlineColor, Uint32 fillColor);
	void drawPiece(int x, int y, int type, int color);
	void drawBoard();
	void drawBackground();
	void update();

	~SDLWindow(); 
private:
	bool loadImages();
	void initColors();
};
