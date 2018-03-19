/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"
#include <istream>

using namespace zetscript;


const char *about="ZetScript 1.3.0 2017-2018 (c) Jordi Espada\n";


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
	bool continue_from_last=true;
	string expression;
	int idx_last_instruction_ok=-1;

	printf("%s",about);

	int g=0;

	const char * instr[]{
		"var g=0;",
		"var f=0;",
		"print(f);",
		0
	};

	int idx_ptr=0;
	CState::saveState();

	do{
		printf("zs>");

		//std::getline(std::cin,expression);
		expression=instr[idx_ptr];

		printf("%s\n",expression.c_str());

		exit = expression=="exit";

		//CState::saveState();
		if(!exit){ // evaluate expression

			CState::clearCurrentCompileInformation();

			if(zetscript->parse(expression)){

				if(zetscript->compile()){

					zetscript->printGeneratedCodeAllClasses();

					if(zetscript->execute(true)){
						//continue_from_last=true;
						CState::saveState();
					}else{
						//CState::restoreLastState();
						//continue_from_last=false;
					}
				}
				else{
					CState::restoreLastState();
					//continue_from_last=false;
				}
			}
			else{
				CState::restoreLastState();
				//continue_from_last=false;
			}
		}

	}while(!exit && (instr[++idx_ptr] != NULL));

	CZetScript::destroy();

#if defined(__ZETSCRIPT_DEBUG__) && defined(__ZETSCRIPT_MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
