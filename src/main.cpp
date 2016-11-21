#include "core/zg_core.h"
#include "SDL2/SDL.h"


void print(const string * s){
	printf("\njjjj %s",s->c_str());
}


int main(int argc, char * argv[]){

	//int result;
	//void *fun_ptr = CScriptClassFactory::new_proxy_function(1,print);

	void *fun_ptr =(void *)( new std::function<int (int)>(std::bind((int (*)(int))print,std::placeholders::_1)));
	string ss="prova";

	printf("prova1 %i %i",sizeof(string *), sizeof(int));

	//auto vv = new std::function<void (string *)>(print);

	//fun_ptr=(void *)vv;


	//vv(&ss);
	(*((std::function<void (string *)> *)fun_ptr))(&ss);

	return 0;



	CLog::setUseAnsiEscape(true);
	CZG_ScriptCore *zg_script = CZG_ScriptCore::getInstance();

	print_info_cr("sizeof(int *):%i sizeof(string *):%i sizeof(int):%i",sizeof(int *),sizeof(string *),sizeof(int));


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
