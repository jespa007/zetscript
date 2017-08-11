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
	/*createSquarePixmap(
	vector<int>{
				 0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff
				,0xffff

			},
			0xffffff	);*/
}




bool CImage::createSquarePixmap(zetscript::CVector * pixmap){
	vector<int> converted_pixmap=zetscript::CVector::cast<int>(pixmap);


	createSquarePixmap(converted_pixmap);
	printf("calling ok\n");
	return true;
}

// create image from native ...
bool CImage::createSquarePixmap(const vector<int> & pixelmap){
	destroy();

	mWidth=pixelmap.size();
	mHeight=mWidth;
	texture = SDL_CreateTexture(  CRender::getInstance()->getRenderer(),SDL_GetWindowPixelFormat( CRender::getInstance()->getWindow() ),SDL_TEXTUREACCESS_TARGET,mWidth,mHeight );

	if(texture!=NULL){
		SDL_SetRenderTarget(CRender::getInstance()->getRenderer(), texture);
		SDL_SetRenderDrawColor(CRender::getInstance()->getRenderer(), 0xFF, 0xFF, 0xFF, 0x17);
		for(int y=0; y < mHeight;y++){
			int pm=pixelmap[y];
			for(int x=0; x < mWidth;x++){
				if(pm & (0x1<<x)){

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
	printf("Image destroyed!");
	if(texture != NULL){
		SDL_DestroyTexture(texture);
	}
	texture=NULL;
}

CImage::~CImage(){
	destroy();
}
