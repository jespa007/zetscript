#include "CSprite.h"

CSprite::CSprite(){
	x=0;
	y=0;
	dx=0;
	dy=0;
}

bool CSprite::checkCollision(CSprite *spr1, CSprite *spr2){
	// checks collision acording current frame...
	return false;
}

void CSprite::addFrame(CImage *img){
	frame.push_back(img);
}

void CSprite::setFrame(int index){

}

void CSprite::update(){
	x+=dx;
	y+=dy;
}

CSprite::~CSprite(){
	for(unsigned i = 0; i < frame.size(); i++){
		delete frame[i];
	}

	frame.clear();
}
