
#pragma once
#include "CZetScript.h"
#include   <SDL2/SDL.h>
#include "CImage.h"
#include "CSprite.h"
#include "CFont.h"





class CRender{

	static CRender *render;
	//SDL_Surface* pScreenSurface = NULL;
	SDL_Renderer *pRenderer = NULL;
	SDL_Event event;

	SDL_Window* pWindow = NULL;

	CRender();
	~CRender();

public:
	static CRender *getInstance();

	static void destroy();



	void createWindow(int width, int height);
	SDL_Renderer *getRenderer();
	SDL_Window *getWindow();

	void clear(Uint8 r, Uint8 g, Uint8 b);

	void drawImage(int x, int y, CImage *img);
	void drawText(int x,int y, CFont * font, string * text);

	void drawSprite(CSprite *spr);

	void update();


};
