#pragma once


#include "CSound.h"

#define MAX_PLAYING_SOUNDS 20

class CSoundPlayer{

	SDL_AudioSpec wav_spec;

	typedef struct {
		Uint8 *audio_pos;
		Uint32 audio_len;
	}tSoundData;

	static tSoundData SoundData[MAX_PLAYING_SOUNDS];

	static CSoundPlayer * singleton;
	SDL_AudioDeviceID dev;
	CSoundPlayer();
	~CSoundPlayer();

	static void CallbackAudio(void *userdata, Uint8* stream, int len);


public:

	static CSoundPlayer * getInstance();
	static void destroy();

	void setup(SDL_AudioFormat format=AUDIO_S16SYS, Uint16 Freq=22050, Uint16 samples=4096, Uint8 channels=2);

	void play(CSound *snd);


};
