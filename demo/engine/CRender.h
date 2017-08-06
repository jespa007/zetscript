
#pragma once
#include "CZetScript.h"
#include   <SDL2/SDL.h>
#include "CImage.h"

#define RMASK32 0x000000ff
#define GMASK32 0x0000ff00
#define BMASK32 0x00ff0000
#define AMASK32 0xff000000



class CRender{

	static CRender *render;
	//SDL_Surface* pScreenSurface = NULL;
	SDL_Renderer *pRenderer = NULL;
	SDL_Event event;

	SDL_Window* pWindow = NULL;

public:
	static CRender *getInstance();

	static void destroy();

	CRender();

	void createWindow(int width, int height);
	SDL_Renderer *getRenderer();
	SDL_Window *getWindow();

	void clear(Uint8 r, Uint8 g, Uint8 b);

	void drawImage(int *x, int *y, CImage *img);
	void drawImage(int x, int y, CImage *img);

	void update();

	~CRender();
};
