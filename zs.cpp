#include "CZetScript.h"

using namespace zetscript;

int main(int argc, char * argv[]) {


	CZetScript *zetscript = CZetScript::getInstance();


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

	if (zetscript->eval_file(argv[1])) {
		zetscript->execute();
	}

	CZetScript::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
