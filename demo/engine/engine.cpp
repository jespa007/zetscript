#include   <SDL2/SDL.h>
#include "CZetScript.h"

using namespace zetscript;



int main(int argc, char *argv[]){

	// SDL stuff
	SDL_Surface* pSurface = NULL;
	SDL_Event event;
	bool quit=false;
	SDL_Window* pWindow = NULL;

	pWindow = SDL_CreateWindow("Invader", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN);

	pSurface = SDL_GetWindowSurface(pWindow);
	SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, 255, 0, 0));

	// here we initiate the script engine ...
	CZetScript *zetscript = CZetScript::getInstance();


	if (argc < 2) {
		printf("Put the game file to parse.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("engine invader.zs");
		printf("\n");
		printf("\n");
		return 0;
	}

	zetscript->eval_file(argv[1]);



	auto init=zetscript->script_call_no_params("init");
	auto update=zetscript->script_call_no_params("update");

	if(init){
		(*init)();
	}


	do{

		// update input
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


		// update script
		if(update){
			(*update)();
		}

	}while(!quit);


	SDL_FreeSurface(pSurface);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();


	return 0;
}
