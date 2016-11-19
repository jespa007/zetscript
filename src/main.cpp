#include "core/zg_core.h"
#include "SDL2/SDL.h"


int main(int argc, char * argv[]){



	CLog::setUseAnsiEscape(false);
	CZG_ScriptCore *zg_script = CZG_ScriptCore::getInstance();


	if(argc < 2){
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}


	char *data_buffer = argv[1];
	ByteBuffer *buffer = NULL;


	if((buffer = CIO_Utils::readFile(argv[1]))!=NULL){

		data_buffer=(char *)buffer->data_buffer;
	}

	if(zg_script->eval(data_buffer)){

		Uint32 t = SDL_GetTicks();
		zg_script->execute();
		print_info_cr("time:%i",SDL_GetTicks()-t);

	}

	CZG_ScriptCore::destroy();

	if(buffer != NULL){
		delete buffer;
}


#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
