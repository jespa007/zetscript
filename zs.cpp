/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"
#include <istream>

using namespace zetscript;



int main(int argc, char * argv[]) {

	CZetScript *zetscript = CZetScript::getInstance();

	if (argc >= 2) {
		zetscript->eval_file(argv[1]);
	}
	else{

		bool exit = false;
		string expression;
		printf("ZetScript %i.%i.%i Copyright (C) 2017-2018 Jordi Espada\n\n"
				,ZETSCRIPT_MAJOR_VERSION
				,ZETSCRIPT_MINOR_VERSION
				,ZETSCRIPT_PATCH_VERSION
				);



		do{
			printf("zs>");
			std::getline(std::cin,expression);

			exit = expression=="exit" || expression=="quit";
			if(!exit){ // evaluate expression
				try{
					zetscript->parse(expression);
					zetscript->compile();
					zetscript->execute();
				}catch(script_error & ex){
					fprintf(stderr,"%s\n",ex.what());
				}
			}

		}while(!exit);// && (instr[++idx_ptr] != NULL));
	}

	CZetScript::destroy();

#ifdef __MEMMANAGER__
	MEM_ViewStatus();
#endif

	return 0;
}
