#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cstdlib>


extern "C" {
#include "SDL.h"
#include "SDL_main.h"
}

#define SCREEN_WIDTH	1080
#define SCREEN_HEIGHT	720
#define NAZWA "Bullethell"

struct SDL_struct {
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
};
struct SDL_images {
	SDL_Surface* player;
	SDL_Surface* bullets;
	SDL_Surface* background;
	SDL_Surface* endgame;
	SDL_Surface* enemy;
	SDL_Surface* penta;
	SDL_Surface* numbers;
};
struct colors_t {
	int czarny;
	int zielony;
	int czerwony;
	int niebieski;
};
struct timer_t {
	int t1, t2, frames, lastState;
	double delta, worldTime, fpsTimer, fps;
};

void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
}

void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
}

void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	if (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT) {
		int bpp = surface->format->BytesPerPixel;
		Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32*)p = color;
	}
}

void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
}

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, int thickness,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	for (i = 0; i < thickness; i++) {
		DrawLine(screen, x + i, y, k, 0, 1, outlineColor);
		DrawLine(screen, x + l - thickness + i, y, k, 0, 1, outlineColor);
		DrawLine(screen, x, y + i, l, 1, 0, outlineColor);
		DrawLine(screen, x, y + k - thickness + i, l, 1, 0, outlineColor);
	}
	for (int i = y + thickness; i < y + k - thickness; i++)
		DrawLine(screen, x + thickness, i, l - 2 * thickness, 1, 0, fillColor);
}

void initSDL(SDL_struct& SDLitems) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(1);
	}
	int rc;
	// tryb pe³noekranowy / fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &SDLitems.window, &SDLitems.renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &SDLitems.window, &SDLitems.renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		exit(1);
	};
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(SDLitems.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(SDLitems.renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(SDLitems.window, NAZWA);


	SDLitems.screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	SDLitems.scrtex = SDL_CreateTexture(SDLitems.renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	SDLitems.charset = SDL_LoadBMP("./cs8x8.bmp");
	if (SDLitems.charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(SDLitems.screen);
		SDL_DestroyTexture(SDLitems.scrtex);
		SDL_DestroyWindow(SDLitems.window);
		SDL_DestroyRenderer(SDLitems.renderer);
		SDL_Quit();
		exit(1);
	};
	SDL_SetColorKey(SDLitems.charset, true, 0x000000);
}

void loadImages(SDL_struct& SDLitems, SDL_images& images) {
	images.player = SDL_LoadBMP("./player.bmp");
	bool error = false;
	if (images.player == NULL) {
		printf("SDL_LoadBMP(player.bmp) error: %s\n", SDL_GetError());
		error = true;
	}
	if (error) {
		SDL_FreeSurface(SDLitems.charset);
		SDL_FreeSurface(SDLitems.screen);
		SDL_DestroyTexture(SDLitems.scrtex);
		SDL_DestroyWindow(SDLitems.window);
		SDL_DestroyRenderer(SDLitems.renderer);
		SDL_Quit();
		exit(1);
	}
}

void clearSDL(SDL_struct& SDLitems) {
	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(SDLitems.charset);
	SDL_FreeSurface(SDLitems.screen);
	SDL_DestroyTexture(SDLitems.scrtex);
	SDL_DestroyRenderer(SDLitems.renderer);
	SDL_DestroyWindow(SDLitems.window);

	SDL_Quit();
}

void initColors(SDL_struct& SDLitems, colors_t& colors) {
	colors.czarny = SDL_MapRGB(SDLitems.screen->format, 0x00, 0x00, 0x00);
	colors.zielony = SDL_MapRGB(SDLitems.screen->format, 0x00, 0xFF, 0x00);
	colors.czerwony = SDL_MapRGB(SDLitems.screen->format, 0xFF, 0x00, 0x00);
	colors.niebieski = SDL_MapRGB(SDLitems.screen->format, 0x11, 0x11, 0xCC);
}

void initTimer(timer_t& timer) {
	timer.t1 = SDL_GetTicks();
	timer.frames = 0;
	timer.fpsTimer = 0;
	timer.fps = 0;
	timer.worldTime = 0;
}

void timerFunction(timer_t& timer) {
	timer.t2 = SDL_GetTicks();
	timer.delta = (timer.t2 - timer.t1) * 0.001;
	timer.t1 = timer.t2;

	timer.worldTime += timer.delta;

	timer.fpsTimer += timer.delta;
	if (timer.fpsTimer > 0.5) {
		timer.fps = timer.frames * 2;
		timer.frames = 0;
		timer.fpsTimer -= 0.5;
	};
}
void drawGameInfo(SDL_struct& SDLitems, timer_t& timer, colors_t& colors) {
	char text[128];
	sprintf(text, "%.0lf FPS", timer.fps);
	DrawString(SDLitems.screen, 10, 10, text, SDLitems.charset);
	sprintf(text, "Czas: %.01lf s", timer.worldTime);
	DrawString(SDLitems.screen, SDLitems.screen->w - strlen(text) * 8, 10, text, SDLitems.charset);
}

void drawFunction(SDL_struct& SDLitems, timer_t& timer, SDL_images& images, colors_t& colors) {
	SDL_FillRect(SDLitems.screen, NULL, colors.czarny);

	drawGameInfo(SDLitems, timer, colors);

	SDL_UpdateTexture(SDLitems.scrtex, NULL, SDLitems.screen->pixels, SDLitems.screen->pitch);
	//		SDL_RenderClear(renderer);
	SDL_RenderCopy(SDLitems.renderer, SDLitems.scrtex, NULL, NULL);
	SDL_RenderPresent(SDLitems.renderer);
}
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	SDL_struct SDLitems;
	SDL_images images;
	colors_t colors;
	bool quit = false;

	srand(time(NULL));
	initSDL(SDLitems);
	initColors(SDLitems, colors);
	loadImages(SDLitems, images);
	clearSDL(SDLitems);

	return 0;
};
