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




bool CImage::createSquarePixmap(zetscript::CVectorScriptVariable * pixmap){
	vector<int> converted_pixmap=zetscript::CVectorScriptVariable::cast<int>(pixmap);


	return createSquarePixmap(converted_pixmap);

}

// create image from native ...
bool CImage::createSquarePixmap(const vector<int> & pixelmap){
	destroy();

	mHeight=pixelmap.size();
	mWidth=-1;

	// get max width...
	for(int y=0; y < mHeight;y++){
		int pm=pixelmap[y];
		for(int x=0; x < 32;x++){
			if(pm & (0x1<<x)){
				mWidth=(mWidth<(x+1)?(x+1):mWidth);
			}
		}
	}

	printf("creating texture map of %ix%i\n",mWidth,mHeight);

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
		fprintf(stderr,"Error creating surface %s\n",SDL_GetError());
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
		printf("Image destroyed!\n");
		SDL_DestroyTexture(texture);
	}
	texture=NULL;
}

CImage::~CImage(){
	destroy();
}
