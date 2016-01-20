#include <QApplication>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "CGraphViewer.h"
#include "CMainWindow.h"

//#include "PointDrawer.h"
//#include "myqtapp.h"



int main(int argc, char *argv[])
{
 

    QApplication app(argc, argv);

  /*  if(SDL_Init(SDL_INIT_AUDIO)< 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;
	 
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}

Mix_Chunk *sound = NULL;
 
sound = Mix_LoadWAV("sound.wav");
if(sound == NULL) {
	fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	exit(-1);
}
int channel;
 
channel = Mix_PlayChannel(-1, sound, 0);
if(channel == -1) {
	fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
}
printf("\nLoaded sound ok!");
while(Mix_Playing(channel) != 0);
 
Mix_FreeChunk(sound);
 
Mix_CloseAudio();
SDL_Quit();*/

    //myQtApp *dialog = new myQtApp;
    //dialog->show();

    CMainWindow main;

     main.resize(300, 200);

     main.show();
    return app.exec();
}
