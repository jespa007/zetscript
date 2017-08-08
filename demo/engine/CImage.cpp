#include "CZetScript.h"
#include "CRender.h"
#include "CInput.h"

SDL_Texture *  CImage::SurfaceToTexture(SDL_Surface *srf){

	SDL_Texture *text=SDL_CreateTextureFromSurface( CRender::getInstance()->getRenderer(), srf );

	if(!text){
		fprintf(stderr,"Error converting surface %s\n",SDL_GetError());
	}

	return text;
}


CImage::CImage(){
	mWidth=	mHeight=0;
	texture=NULL;
	// default pixmap..
	createSquarePixmap(
	vector<char>{
			  1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
			 ,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			 ,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
			 ,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
			},
			0xffffff	);
}

CImage::CImage(const vector<char> & pixmap, Uint32 color){
	texture=NULL;
	createSquarePixmap(pixmap,color);
}

// create image from script ...
bool CImage::createSquarePixmap(zetscript::CVector * pixelmap, int *color){


}



// create image from native ...
bool CImage::createSquarePixmap(const vector<char> & pixelmap, Uint32 color){
	destroy();

	mWidth=sqrt(pixelmap.size());
	mHeight=mWidth;
	texture = SDL_CreateTexture(  CRender::getInstance()->getRenderer(),SDL_GetWindowPixelFormat( CRender::getInstance()->getWindow() ),SDL_TEXTUREACCESS_TARGET,mWidth,mHeight );

	if(texture!=NULL){
		SDL_SetRenderTarget(CRender::getInstance()->getRenderer(), texture);
		SDL_SetRenderDrawColor(CRender::getInstance()->getRenderer(), 0xFF, 0xFF, 0xFF, 0x17);
		for(int y=0; y < mHeight;y++){
			for(int x=0; x < mWidth;x++){
				if(pixelmap[y*mWidth+x]){
					SDL_RenderDrawPoint(CRender::getInstance()->getRenderer(), x, y);
				}
			}
		}

		// this sets renderer back to default target (the window)
		SDL_SetRenderTarget(CRender::getInstance()->getRenderer(), NULL);
	}
	else{
		fprintf(stderr,"Error creating surface %s",SDL_GetError());
		return false;
	}
	return true;
}

SDL_Texture * CImage::getTexture(){
	return texture;
}

int CImage::getWidth(){
	return mWidth;
}

int CImage::getHeight(){
	return mHeight;
}

void CImage::destroy(){
	if(texture != NULL){
		SDL_DestroyTexture(texture);
	}
	texture=NULL;
}

CImage::~CImage(){
	destroy();
}
