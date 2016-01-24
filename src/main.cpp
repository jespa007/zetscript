#include "script/zg_script.h"





int main(int argc, char * argv[]){
	

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

	CZG_Script::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
