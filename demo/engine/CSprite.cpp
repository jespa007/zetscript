#include "CSprite.h"

CSprite::CSprite(){
	x=0;
	y=0;
	dx=0;
	dy=0;
	currentframe=0;
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

CImage *CSprite::getCurrentFrame(){
	if(currentframe<frame.size()){
		return frame[currentframe];
	}

	return NULL;
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
