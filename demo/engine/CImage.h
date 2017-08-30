
#pragma once

#include   <SDL2/SDL.h>
#include <CZetScript.h>

class CImage{

protected:
	SDL_Texture *texture;
	void destroy();
	int mWidth,mHeight;

	static SDL_Texture * SurfaceToTexture(SDL_Surface *srf);
	bool createSquarePixmap(const vector<int> & pixmap);
public:
	CImage();
	CImage(const vector<int> & pixmap);

	SDL_Texture *getTexture();

	// create image from script ...
	bool createSquarePixmap(zetscript::CVectorScriptVariable *vector);


	int getWidth();
	int getHeight();

	~CImage();
};
