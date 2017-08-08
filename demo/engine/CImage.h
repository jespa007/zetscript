
#pragma once

#include   <SDL2/SDL.h>
#include <CZetScript.h>

class CImage{

protected:
	SDL_Texture *texture;
	void destroy();
	int mWidth,mHeight;

	static SDL_Texture * SurfaceToTexture(SDL_Surface *srf);

public:
	CImage();
	CImage(const vector<char> & pixmap, Uint32 color);

	SDL_Texture *getTexture();

	// create image from script ...
	bool createSquarePixmap(zetscript::CVector *vector, int *color);
	bool createSquarePixmap(const vector<char> & pixmap, Uint32 color);

	int getWidth();
	int getHeight();

	~CImage();
};
