#pragma once

#include   <SDL2/SDL.h>
#include <CZetScript.h>

class CSound{


public:

	Uint32 wav_length;
	Uint8 *wav_buffer;


	CSound();

	bool load(string * file);

	~CSound();

};

