#include "SDLgraphics.hpp"
#include <string> 

#pragma warning(disable: 26451)

SDLWindow::SDLWindow(int width, int height, std::string_view title) : w(width), h(height), title(title) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return;
	}
	int rc;
	// tryb pełnoekranowy / fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return;
	}
	if (!loadImages()) {
		SDL_Quit();
		printf("BMP loading error: %s\n", SDL_GetError());
		return;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, width, height);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, title.data());


	screen = SDL_CreateRGBSurface(0, width, height, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);

	TTF_Init();

	// !wyłączenie widoczności kursora myszy
	SDL_ShowCursor(SDL_ENABLE);
	initColors();
}

SDLWindow::~SDLWindow() {
	SDL_FreeSurface(images.charset);
	SDL_FreeSurface(images.pieces);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();
}

Uint32 SDLWindow::mapColor(int rgb) const {
	return SDL_MapRGB(screen->format, (rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, (rgb) & 0xFF );
}

std::pair<int, int> SDLWindow::mapClick(int x, int y) const {
	return std::make_pair((x - boardX()) / tileSize, (y - boardY()) / tileSize);
}

void SDLWindow::drawPixel(int x, int y, Uint32 color) {
	int bpp = screen->format->BytesPerPixel;
	Uint8* p = (Uint8*)screen->pixels + (y * screen->pitch + x * bpp);
	*(Uint32*)p = color;
}

void SDLWindow::drawLine(int x, int y, int len, int dx, int dy, Uint32 color) {
	for (int i = 0; i < len; i++) {
		drawPixel(x, y, color);
		x += dx;
		y += dy;
	};
}

void SDLWindow::drawRectangle(SDL_Rect rect, Uint32 fillColor) {
	SDL_FillRect(screen, &rect, fillColor);
}

void SDLWindow::drawCircle(int x, int y, int radius, int thickness, Uint32 fillColor) {
	for (int i = std::max(y - radius, 0); i < std::min(y + radius, h); i++) {
		for (int j = std::max(x - radius, 0); j < std::min(x + radius, w); j++) {
			int distance = ((j - x) * (j - x) + (i - y) * (i - y));
			if (distance < radius * radius && distance > (radius-thickness)*(radius-thickness))
				drawPixel(j, i, fillColor);
		}
	}
}

void SDLWindow::drawBackground() {
	SDL_BlitSurface(images.background, NULL, screen, NULL);
}

void SDLWindow::drawRectangle(SDL_Rect rect, int thickness, Uint32 outlineColor, Uint32 fillColor) {
	SDL_Rect borders[]{
		{ rect.x, rect.y, thickness, rect.h },
		{ rect.x, rect.y, rect.w, thickness },
		{ rect.x + rect.w - thickness, rect.y, thickness, rect.h },
		{ rect.x, rect.y + rect.h - thickness, rect.w, thickness },
	};

	SDL_FillRect(screen, &rect, fillColor);
	for (const auto &border : borders)
		SDL_FillRect(screen, &border, outlineColor);
}
void SDLWindow::drawPiece(int x, int y, int type, int color) {
	int px = type * pieceSize, py = color * pieceSize;
	SDL_Rect
		s{ px, py, pieceSize, pieceSize },
		d{ x * tileSize + boardX(), y * tileSize + boardY(), pieceSize, pieceSize };
	SDL_BlitSurface(images.pieces, &s, screen, &d);
}

void SDLWindow::drawBoard() {
	constexpr int frameThick = 20;
	const int x = boardX(), y = boardY();
	drawRectangle({ x-frameThick,y-frameThick,tileSize * boardSize + 2*frameThick, tileSize * boardSize + 2*frameThick}, frameThick, colors.dark_gary, colors.white);
	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {
			if (i % 2 != j % 2) {
				drawRectangle({ x + j * tileSize, y + i * tileSize, tileSize, tileSize }, colors.light_gary);
			}
		}
	}
	for (int i = 0; i < boardSize; i++) {
		drawString(x - 9 + (i+1)*tileSize, y - 15 + boardSize * tileSize, std::string(1, 'a' + i), 15, Fonts::ARIAL, { 0, 0, 0 });
		drawString(x + 1, y + 1 + i*tileSize, std::to_string(8-i), 15, Fonts::ARIAL, { 0, 0, 0 });
	}
}

void SDLWindow::drawChoice(int x, int y) {
	for (int i = -1; i < 3; i++) {
		drawRectangle({ (x + i) * tileSize + boardX(), y * tileSize + boardY(), tileSize, tileSize }, 2, colors.dark_gary, colors.light_gary);
	}
}

void SDLWindow::update() {
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool SDLWindow::loadImages() {
	images.charset = SDL_LoadBMP(charsetName);
	images.pieces = SDL_LoadBMP(piecesName);
	images.background = SDL_LoadBMP(backgroundName);
	return (images.charset && images.pieces && images.background);
}

void SDLWindow::initColors() {
	colors.white = mapColor(0xDDDDDD);
	colors.light_gary = mapColor(0x7E7E7E);
	colors.dark_gary = mapColor(0x333333);
	colors.light_red = mapColor(0xFFAAAA);
	colors.light_green = mapColor(0xAAFFAA);
}

void SDLWindow::highlightTile(int xp, int yp, Highlight type) {
	const int x = boardX(), y = boardY();
	switch (type)
	{
	case Highlight::RED:
		drawRectangle({ x + xp * tileSize, y + yp * tileSize, tileSize, tileSize }, colors.light_red);
		break;
	case Highlight::GREEN:
		drawRectangle({ x + xp * tileSize, y + yp * tileSize, tileSize, tileSize }, colors.light_green);
		break;
	case Highlight::CIRCLE:
		drawCircle(x + (xp+0.5) * tileSize, y + (yp+0.5) * tileSize, tileSize / 4, 5, colors.dark_gary);
		break;
	}
}
void SDLWindow::drawString(int x, int y, std::string text, int fontSize, Fonts fontName, SDL_Color color) {
	switch (fontName)
	{
	case Fonts::SANS:
		font = TTF_OpenFont("./fonts/sans.ttf", fontSize);
		break;
	case Fonts::COMIC_SANS:
		font = TTF_OpenFont("./fonts/comic_sans.ttf", fontSize);
		break;
	default:
	case Fonts::ARIAL:
		font = TTF_OpenFont("./fonts/arial.ttf", fontSize);
		break;
	}
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Rect textRect;
	textRect.x = x;
	textRect.y = y;
	SDL_BlitSurface(surface, NULL, screen, &textRect);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
}