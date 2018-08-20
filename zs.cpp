/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"
#include <istream>

using namespace zetscript;


int main(int argc, char * argv[]) {


	CZetScript *zetscript = CZetScript::getInstance();

	if (argc > 1) {
		bool execute=true;
		bool show_bytecode=false;
		string file="";

		for(int i=1; i < argc; i++){

			vector<string> a=CZetScriptUtils::split(argv[i],'=');
			switch(a.size()){
			case 1:

				if(strcmp(argv[i],"--no_execute")==0){
					execute = false;
				}else if(strcmp(argv[i],"--show_bytecode")==0){
					show_bytecode=true;

				}else if(strcmp(argv[i],"--version")==0){
					printf("ZetScript %i.%i.%i Copyright (C) 2016-2018\n",ZETSCRIPT_MAJOR_VERSION,ZETSCRIPT_MINOR_VERSION,ZETSCRIPT_PATCH_VERSION);
					exit(0);
				}else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			case 2:

				if(strcmp(a[0].c_str(),"--file")==0){
					file=a[1].c_str();
				}
				else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			default:
				fprintf(stderr,"invalid argument %s\n",argv[i]);
				exit(-1);
				break;
			}

		}

		if(file==""){
			fprintf(stderr,"Program with arguments you must specify file (i.e --file=filename )\n");
			exit(-1);
		}

		try{
			zetscript->eval_file(file.c_str(),execute,show_bytecode);
		}catch(script_error & error){
			fprintf(stderr,"%s\n",error.what());
		}
		catch(std::exception & error){
			fprintf(stderr,"%s\n",error.what());
		}
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
