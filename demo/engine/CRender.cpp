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

void CRender::drawImage(int x, int y, CImage *img){
	//Apply the image
	SDL_Texture *text=img->getTexture();
	if(text){
		SDL_Rect rect={x,y,img->getWidth(),img->getHeight()};
		SDL_RenderCopy(pRenderer, text, NULL, &rect);
	}
}

void CRender::drawText(int x,int y, CFont * font, string * text){
	SDL_Texture *font_text=font->getTexture();
	if(font_text){

		SDL_Rect rect={x,y,font->getCharWidth(),font->getCharHeight()};
		for(unsigned i=0; i < text->size(); i++){
			char c=text->at(i);
			SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c), &rect);
			rect.x+=rect.w;
		}


	}
}

void CRender::update(){
	//Update screen
	SDL_RenderPresent( pRenderer );
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
