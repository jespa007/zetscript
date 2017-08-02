#include   <SDL2/SDL.h>
#include "CZetScript.h"

using namespace zetscript;

SDL_Surface* pSurface = NULL;
SDL_Event event;
bool quit=false;
SDL_Window* pWindow = NULL;

void init(){
	pWindow = SDL_CreateWindow("Invader", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN);

	pSurface = SDL_GetWindowSurface(pWindow);
	SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, 255, 0, 0));
}

void input_update(){
	if( SDL_PollEvent( &event ) )
		{
		  /*  if( event.type == SDL_MOUSEMOTION )
			{
				mx = event.motion.x;
				my = event.motion.y;
			}

			if( event.type == SDL_MOUSEBUTTONDOWN )
			{
				if( hut.getselected() && hut.getplacable() )
				{
					hut.place( map );
				}
			}*/

			if( event.type == SDL_QUIT )
			{
				quit = true;
			}

			switch( event.key.keysym.sym )
			{
			case SDLK_ESCAPE: quit = true; break;
			}
		}
}

bool *pressed_quit(){
	return &quit;
}


void deinit(){
	SDL_FreeSurface(pSurface);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char *argv[]){


	CZetScript *zetscript = CZetScript::getInstance();


	if(!register_C_Function(init)) return false;
	if(!register_C_Function(pressed_quit)) return false;
	if(!register_C_Function(deinit)) return false;
	if(!register_C_Function(input_update)) return false;

	zetscript->eval_file("invader.zs");

	return 0;
}
