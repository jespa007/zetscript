#include "CSprite.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


CSprite::CSprite(){
	x=0;
	y=0;
	dx=0;
	dy=0;
	currentframe=0;
	current_time_frame=0;
	time_frame=2000;
	width= height=0;
}

bool CSprite::checkCollision(CSprite *spr1, CSprite *spr2){

	tFrameInfo *img1=spr1->getCurrentFrame();
	tFrameInfo *img2=spr2->getCurrentFrame();

	int x11=spr1->x
	   ,x12=x11+img1->image->getWidth()
	   ,y11=spr1->y
	   ,y12=y11+img1->image->getHeight();

	int x21=spr2->x
	   ,x22=x21+img2->image->getWidth()
	   ,y21=spr2->y
	   ,y22=y21+img2->image->getHeight();

	if((
		(x11 < x22 && x12 > x21)// || (x21>=x11 && x21 <  x12)
	   )
			&&
	   (
		(y11 < y22 && y12 > y21)// || ( y12 > y11 && y12 < y22)
       )
	   ){
		return true;
	   }

	return false;
}

void CSprite::addFrame(CImage *img, int rgb){

	tFrameInfo fi={img,(Uint32)rgb};

	frame.push_back(fi);

	if(frame.size()==1){ // was the first
		width = frame[0].image->getWidth();
		height = frame[0].image->getHeight();
	}

}

void CSprite::setFrame(int index){

}

void CSprite::setTimeFrame(int time){
	time_frame=time;
}

CSprite::tFrameInfo *CSprite::getCurrentFrame(){
	if(currentframe<frame.size()){
		return &frame[currentframe];
	}

	return NULL;
}

void CSprite::update(){
	x+=dx;
	y+=dy;

	if(current_time_frame<SDL_GetTicks()){
		currentframe=(currentframe+1);
		if(currentframe>=frame.size()){
			currentframe=0;
		}

		current_time_frame=SDL_GetTicks()+time_frame;

		width = frame[currentframe].image->getWidth();
		height = frame[currentframe].image->getHeight();

	}
}

CSprite::~CSprite(){

	frame.clear();
}
