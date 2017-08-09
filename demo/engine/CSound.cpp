#include "CSound.h"


CSound::CSound(){

	wav_buffer=0;
	wav_length=0;

}

bool CSound::load(string *file){

	SDL_AudioSpec wav_spec;

	if(SDL_LoadWAV(file->c_str(),
					&wav_spec,
					 &wav_buffer,
					  &wav_length) ==NULL){
		fprintf(stderr, "%s\n", SDL_GetError());
		return false;
	}

	return true;
}


CSound::~CSound(){
	if(wav_buffer){
		SDL_FreeWAV(wav_buffer);
	}
}
