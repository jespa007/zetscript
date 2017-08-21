
#pragma once
#include "CZetScript.h"
#include   <SDL2/SDL.h>
#include "CImage.h"
#include "CSprite.h"
#include "CFont.h"





class CRender{

	int m_width, m_height;

	static CRender *render;
	//SDL_Surface* pScreenSurface = NULL;
	SDL_Renderer *pRenderer = NULL;
	SDL_Event event;
	bool fullscreen;

	SDL_Window* pWindow = NULL;

	CRender();
	~CRender();

public:
	static CRender *getInstance();

	static void destroy();


	int getWidth();
	int getHeight();

	void createWindow(int width, int height);
	void toggleFullscreen();
	SDL_Renderer *getRenderer();
	SDL_Window *getWindow();

	void clear(Uint8 r, Uint8 g, Uint8 b);

	void drawImage(int x, int y, CImage *img, int color);
	void drawText(int x,int y, CFont * font, string * text);

	void drawSprite(CSprite *spr);

	void update();


};
