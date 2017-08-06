
#pragma once

#include   <SDL2/SDL.h>
#include   "def_keys.h"



#define  T_ESC	      	CInput::getInstance()->key[KEY_ESCAPE]

#define  TR_UP          CInput::getInstance()->keyR[KEY_UP]
#define  TR_LEFT        CInput::getInstance()->keyR[KEY_LEFT]
#define  TR_RIGHT       CInput::getInstance()->keyR[KEY_RIGHT]
#define  TR_DOWN        CInput::getInstance()->keyR[KEY_DOWN]

typedef struct{
	Uint32 codeKey;
	bool pressUp;
	bool pressDown;
	bool pressLeft;
	bool pressRight;
	bool pressEscape;
	bool pressReturn;
	bool pressBackSpace;
	bool pressHome;
	bool pressEnd;
}tEventKey,tEventRepeatKey;


class CInput{

	static CInput *input;

	SDL_Event Event;


public:
	bool            key[KEY_LAST];
	bool            keyR[KEY_LAST];

	/*static bool *zs_pressedEsc(){
		return &T_ESC;
	}*/



	static CInput * getInstance();

	static void destroy();

	void update();

};
