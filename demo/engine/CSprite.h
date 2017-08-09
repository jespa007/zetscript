#pragma once

#include "CImage.h"

class CSprite{

public:
	int x, y;
	int dx, dy;
	std::vector<CImage *> frame;

	static bool checkCollision(CSprite *spr1, CSprite *spr2);

	CSprite();

	void addFrame(CImage *fr);
	void setFrame(int n);

	void update();

	~CSprite();

};
