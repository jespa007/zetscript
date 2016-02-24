#include "script/zg_script.h"





int main(int argc, char * argv[]){
	
	int i=0;
	i=10*-2;



	i=i---i;

	if(argc < 2){
		printf("Put expression to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("5*6+3");
		printf("\n");
		printf("\n");
		return 0;
	}
	
	CZG_Script *zg_script = CZG_Script::getInstance();
	zg_script->init();

	zg_script->eval(argv[1]);

	print_info_cr("sizeobject:%i",sizeof(CObject));
	print_info_cr("sizenumber:%i",sizeof(CNumber));

	print_info_cr("float:%s",typeid(float).name());
	print_info_cr("string:%s",typeid(string).name());
	print_info_cr("bool:%s",typeid(bool).name());


	CZG_Script::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
