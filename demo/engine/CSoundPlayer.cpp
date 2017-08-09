#include "CSoundPlayer.h"

CSoundPlayer *CSoundPlayer::singleton=NULL;
CSoundPlayer::tSoundData CSoundPlayer::SoundData[MAX_PLAYING_SOUNDS]={0};


void CSoundPlayer::CallbackAudio(void *userdata, Uint8* stream, int len)
{
	//
	memset(stream,0,len);

	for(int i = 0; i < MAX_PLAYING_SOUNDS; i++){
		if(SoundData[i].audio_len!=0){ // some data to play...
			int length = (int)SoundData[i].audio_len<len?SoundData[i].audio_len:len;
			SDL_MixAudio(stream, SoundData[i].audio_pos, len, SDL_MIX_MAXVOLUME);

			SoundData[i].audio_len-=length;
			SoundData[i].audio_pos+=length;
		}
	}
}



CSoundPlayer *CSoundPlayer::getInstance(){
	if(singleton==NULL){
		memset(SoundData,0,sizeof(SoundData));
		singleton=new CSoundPlayer();
	}
	return singleton;

}

void CSoundPlayer::destroySingletons(){
	if(singleton != NULL){
		delete singleton;
	}

	singleton = NULL;

}


CSoundPlayer::CSoundPlayer(){
	dev=0;
}



void CSoundPlayer::setup(SDL_AudioFormat format, Uint16 Freq, Uint16 samples, Uint8 channels){

	if (SDL_WasInit(SDL_INIT_AUDIO) != SDL_INIT_AUDIO) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
			fprintf(stderr,"Unable to init sound subsystem: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	//SDL_AudioSpec have;

	SDL_memset(&wav_spec, 0, sizeof(wav_spec)); /* or SDL_zero(want) */
	//wav_spec.freq = Freq;
	//wav_spec.format = format;
	//wav_spec.channels = channels;
	//wav_spec.samples = samples;
	wav_spec.userdata=NULL;
	wav_spec.callback = CallbackAudio; /* you wrote this function elsewhere. */

	//if ((dev = SDL_OpenAudioDevice(NULL, 0, &wav_spec, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE))==0) {
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
	    fprintf(stderr,"Failed to open audio: %s", SDL_GetError());
	} else {
	    /*if (have.format != wav_spec.format) {
	        fprintf(stderr,"We didn't get audio format.");
	    }*/
	    SDL_PauseAudio(0); /* start audio playing. */

	    printf("SoundPlayer initialized!\n");
	}
}

void CSoundPlayer::play(CSound *snd){
	for(int i = 0; i < MAX_PLAYING_SOUNDS; i++){
		if(SoundData[i].audio_len==0){ // is not playing.
			SoundData[i].audio_pos=snd->wav_buffer;
			SoundData[i].audio_len=snd->wav_length;
			return;
		}
	}
}

CSoundPlayer::~CSoundPlayer(){
	// stop audio...
	SDL_PauseAudio(1);
    SDL_CloseAudio();
}
