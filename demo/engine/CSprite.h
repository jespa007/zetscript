#pragma once

#include "CImage.h"

class CSprite{
	unsigned currentframe;
	Uint32 current_time_frame,time_frame;

public:

	typedef struct{
		CImage *image;
		Uint32 color;
	}tFrameInfo;

	int x, y;
	int dx, dy;
	int width, height;
	std::vector<tFrameInfo> frame;

	static bool checkCollision(CSprite *spr1, CSprite *spr2);

	CSprite();

	void addFrame(CImage *fr, int rgb);
	void setFrame(int n);
	void setTimeFrame(int time);
	int getWidth();
	int getHeight();
	CSprite::tFrameInfo * getCurrentFrame();

	void update();

	~CSprite();

};
