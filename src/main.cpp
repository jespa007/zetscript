#include "script/zg_script.h"
#include "SDL2/SDL.h"


class a{
public:
	int b;

	a(){b=0;}
};



int pass_string ( int s_int){

	string s = *((string *)&s_int);
	print_info_cr("HHHHHH: %s", s.c_str());


	s=STR(hola);

	print_info_cr("HHHHHH2: %s", s.c_str());

}

int main(int argc, char * argv[]){

	string s = "hola!";
	int s_int= *((int *)& s);

	pass_string(s_int);
	int yy[] = {0,1,2,3,4};

	--yy[0]+yy[0]++;

	a a;

	a .

	b = 1;






	printf("%s\n",(char *) typeid(CString *).name());
	printf("%s\n",(char *) typeid(string *).name());


	int i=1+0==1?2:3*2+10 ? 0 : 1;

	i=+--i;

	++i=0;





	CLog::setUseAnsiEscape(true);

	printf("\nvar %i\n\n",i);

	print_info_cr("sizeof(CObject)=%i sizeof(float)=%i sizeof(string)=%i",sizeof(CObject),sizeof(float),sizeof(string));



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
	ByteBuffer *buffer = CIO_Utils::readFile(argv[1]);

	if(buffer != NULL){

		CZG_Script *zg_script = CZG_Script::getInstance();

			if(zg_script->eval((char *)buffer->data_buffer)){
				/*for(i=0; i < 20;i++)
				{
					Uint32 t = SDL_GetTicks();*/
					//zg_script->execute();
					/*print_info_cr("%i",SDL_GetTicks()-t);
				}*/
			}

		print_info_cr("sizeobject:%i",sizeof(CObject));
		print_info_cr("sizenumber:%i",sizeof(CNumber));

		print_info_cr("float:%s",typeid(float).name());
		print_info_cr("string:%s",typeid(string).name());
		print_info_cr("bool:%s",typeid(bool).name());


		CZG_Script::destroy();
		delete buffer;
	}

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
