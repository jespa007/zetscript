/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"
#include <istream>

using namespace zetscript;


const char *about="ZetScript 2017 © Jordi Espada\n";


int main(int argc, char * argv[]) {

	CZetScript *zetscript = CZetScript::getInstance();


	/*if (argc < 2) {
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}


	if(!zetscript->eval_file(argv[1])){
		fprintf(stderr,"%s\n",ZS_GET_ERROR_MSG());
	}*/

	bool exit = false;
	string expression;


	printf("%s",about);

	do{
		printf(">");

		std::getline(std::cin,expression);

		exit = expression=="exit";

		if(!exit){ // evaluate expression

			CState::clearCurrentCompileInformation();

			if(zetscript->parse(expression)){
				if(zetscript->compile()){
					if(zetscript->execute()){
						//CState::saveState();
					}else{
						//CState::restoreLastState();
					}
				}
				else{
					//CState::restoreLastState();
				}
			}
			else{
				//CState::restoreLastState();
			}
		}

	}while(!exit);

	CZetScript::destroy();

#if defined(__ZETSCRIPT_DEBUG__) && defined(__ZETSCRIPT_MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
