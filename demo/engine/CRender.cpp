#include "CRender.h"

#define RMASK32 0x000000ff
#define GMASK32 0x0000ff00
#define BMASK32 0x00ff0000
#define AMASK32 0xff000000



CRender *CRender::render=NULL;

CRender *CRender::getInstance(){
	if(render == NULL){
		render = new CRender();
	}
	return render;
}

void CRender::destroy(){
	if(render){
		delete render;
	}
	render = NULL;
}

CRender::CRender(){
	pWindow = NULL;
	pRenderer=NULL;
	fullscreen=false;
	m_height=0;
	m_width=0;
}

void CRender::createWindow(int width, int height){

	if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO) {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr,"Unable to init video subsystem: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	pWindow = SDL_CreateWindow(
			"Engine"
		    ,SDL_WINDOWPOS_UNDEFINED
			,SDL_WINDOWPOS_UNDEFINED
			,width
			,height
			,0);

	this->m_width=width;
	this->m_height=height;

	if (!pWindow) {
		fprintf(stderr,"Unable to create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_WINDOW_SHOWN);

    if (!pRenderer) {
		fprintf(stderr,"Unable to create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
    }
}

int CRender::getWidth(){
	return m_width;
}

int CRender::getHeight(){
	return m_height;
}


SDL_Renderer * CRender::getRenderer(){
	return pRenderer;
}

SDL_Window * CRender::getWindow(){
	return pWindow;
}

void CRender::clear(Uint8 r, Uint8 g, Uint8 b){
	//Clear screen
	SDL_SetRenderDrawColor( pRenderer, r, g, b, 0xFF );
	SDL_RenderClear( pRenderer );
}

void CRender::drawImage(int x, int y, CImage *img, int rgb){
	//Apply the image
	if(img){
		SDL_Texture *text=img->getTexture();
		if(text){
			SDL_Rect rect={x-(img->getWidth()>>1),y-(img->getHeight()>>1),img->getWidth(),img->getHeight()};

			//SDL_SetRenderDrawColor(CRender::getInstance()->getRenderer(), 0xFF, 0x00, 0xFF, 0x17);
			SDL_SetTextureColorMod(text,
			                           rgb&0xFF,
			                           (rgb>>8)&0xFF,
			                           (rgb>>16)&0xFF);


			SDL_RenderCopy(pRenderer, text, NULL, &rect);
		}
	}
}

void CRender::drawText(int x,int y, CFont * font, string * text){
	if(font){
		SDL_Texture *font_text=font->getTexture();
		if(font_text){

			int total_width=font->getTextWith(*text);

			x-=total_width>>1;

			SDL_Rect rect={x,y,font->getCharWidth(),font->getCharHeight()};
			for(unsigned i=0; i < text->size(); i++){
				char c=text->at(i);
				SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c), &rect);
				rect.x+=rect.w;
			}
		}
	}
}

void CRender::drawSprite(CSprite *spr){
	CSprite::tFrameInfo *fi=spr->getCurrentFrame();
	if(fi!=NULL){
		drawImage(spr->x,spr->y, fi->image, fi->color);
	}
}

void CRender::update(){
	//Update screen
	SDL_RenderPresent( pRenderer );
}

void CRender::toggleFullscreen(){
	if(!fullscreen){
		SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN);
	}else{
		SDL_SetWindowFullscreen(pWindow, 0);
	}

	fullscreen=!fullscreen;
}

CRender::~CRender(){
	if(pRenderer != NULL){
		SDL_DestroyRenderer(pRenderer);
	}

	if(pWindow != NULL){
		SDL_DestroyWindow(pWindow);
	}

	pRenderer=NULL;
	pWindow=NULL;
	SDL_Quit();
}
