#include "CZetScript.h"


int main(int argc, char * argv[]) {


	CZetScript *zet_script = CZetScript::getInstance();


	if (argc < 2) {
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}

	if (zet_script->eval_file(argv[1])) {
		zet_script->execute();
	}

	CZetScript::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
