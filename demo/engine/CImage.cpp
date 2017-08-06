#include "CZetScript.h"
#include "CRender.h"
#include "CInput.h"

CImage::CImage(){
	//mWidth=	mHeight=0;
	//texture=NULL;
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



	//SDL_Surface * srf = SDL_CreateRGBSurface(0, mWidth, mHeight, 32, RMASK32, GMASK32, BMASK32, AMASK32);
	//SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( srf, SDL_GetWindowPixelFormat( CRender::getInstance()->getWindow() ), NULL );
	//texture = SDL_CreateTextureFromSurface(CRender::getInstance()->getRenderer(), formattedSurface,);

	//gRenderer, SDL_GetWindowPixelFormat( gWindow ), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h
	texture = SDL_CreateTexture(  CRender::getInstance()->getRenderer(),SDL_GetWindowPixelFormat( CRender::getInstance()->getWindow() ),SDL_TEXTUREACCESS_TARGET,mWidth,mHeight );

	if(texture!=NULL){
		//void *pixel;
		//int mPitch;
		/*if(!SDL_LockTexture( texture, NULL, (void **)&pixel, &mPitch )){
			fprintf(stderr,"Error cannot lock surface: %s",SDL_GetError());
			return false;
		}*/

		/*SDL_PixelFormat * fmt = SDL_AllocFormat(SDL_GetWindowPixelFormat(CRender::getInstance()->getWindow()));


		for(int offset = 0; offset < mWidth*mHeight; offset++){
			((Uint32 *)pixel)[offset] = SDL_MapRGB(fmt,255,0,0);//pixelmap[offset]?(color ^ AMASK32):0;
		}*/

		SDL_SetRenderTarget(CRender::getInstance()->getRenderer(), texture);
		SDL_SetRenderDrawColor(CRender::getInstance()->getRenderer(), 0xFF, 0xFF, 0xFF, 0x17);
		SDL_Color pcolor = {0, 255, 0, 0};
		for(int y=0; y < mHeight;y++){
			for(int x=0; x < mWidth;x++){
				if(pixelmap[y*mWidth+x]){
					SDL_RenderDrawPoint(CRender::getInstance()->getRenderer(), x, y);
				}
			}
		}


		// this sets renderer back to default target (the window)
		SDL_SetRenderTarget(CRender::getInstance()->getRenderer(), NULL);


		 //SDL_UnlockTexture( texture );
		 //SDL_FreeSurface(srf);

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
