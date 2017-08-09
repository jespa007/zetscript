#pragma once

#include "CImage.h"

class CFont:public CImage{

	int totalchars_x, totalchars_y,totalchars;
	SDL_Rect m_aux_rect;
	int char_width,char_height;
public:

	CFont();

	bool load(string * file,int char_width,int char_height);

	int getCharWidth();
	int getCharHeight();
	SDL_Rect * getRectChar(char c);

	~CFont();

};
