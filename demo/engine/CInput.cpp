#include "CInput.h"

CInput *CInput::input=NULL;

CInput * CInput::getInstance(){
	if(input == NULL){
		input = new CInput();
	}
	return input;
}

void CInput::destroy(){
	if(input){
		delete input;
	}
	input = NULL;
}

void CInput::update(){

	memset(key,0,sizeof(key));
	Uint32 m_idxKey=-1;
	tEventKey the_keyEvent;


	while( SDL_PollEvent( &Event ) )
	{
		memset(&the_keyEvent,0,sizeof(the_keyEvent));

		if(Event.key.repeat == 0){ // one key key press at time...

			switch(Event.type) {
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				m_idxKey = Event.key.keysym.sym;

				//print_info_cr("UnPress");
				switch(Event.key.keysym.sym){

					case SDLK_UP: m_idxKey = KEY_UP; the_keyEvent.pressUp = true;break;
					case SDLK_DOWN: m_idxKey = KEY_DOWN; the_keyEvent.pressDown = true;break;
					case SDLK_LEFT: m_idxKey = KEY_LEFT; the_keyEvent.pressLeft = true;break;
					case SDLK_RIGHT:m_idxKey = KEY_RIGHT; the_keyEvent.pressRight = true;break;
					case SDLK_ESCAPE: the_keyEvent.pressEscape = true; break;
					case SDLK_F5: m_idxKey = KEY_F5;the_keyEvent.pressF5 = true; break;
					case SDLK_F9: m_idxKey = KEY_F9;the_keyEvent.pressF9 = true; break;
					case SDLK_BACKSPACE:m_idxKey=KEY_SPACE;the_keyEvent.pressBackSpace = true;break;

					default:
						if(KEY_SPACE <= Event.key.keysym.sym && Event.key.keysym.sym <= KEY_z) {
							the_keyEvent.codeKey = Event.key.keysym.sym;
						}

						break;
				}

				if(m_idxKey<KEY_LAST) {

					keyR[m_idxKey]=false;

					if(Event.type == SDL_KEYDOWN) {
						key[m_idxKey]=true;
						keyR[m_idxKey]=true;

					}

				}

				break;

			}
		}
	}
}
