/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZETSCRIP_COPYRIGHT "ZetScript %i.%i.%i Copyright (C) 2016-2020 Jordi Espada\n",ZETSCRIPT_MAJOR_VERSION,ZETSCRIPT_MINOR_VERSION,ZETSCRIPT_PATCH_VERSION

using namespace zetscript;


int main(int argc, char * argv[]) {

	ZetScript *zs = new ZetScript();


	if (argc > 1) {
		bool vm_execute=true;
		bool show_bytecode=false;
		std::string file;

		for(int i=1; i < argc; i++){

			std::vector<std::string> a=zs_strutils::split(argv[i],'=');
			switch(a.size()){
			case 1:

				if(strcmp(argv[i],"--no_execute")==0){
					vm_execute = false;
				}else if(strcmp(argv[i],"--show_code")==0){
					show_bytecode=true;

				}else if(strcmp(argv[i],"--version")==0){
					printf(ZETSCRIP_COPYRIGHT);
					exit(0);
				}else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			case 2:

				if(strcmp(a[0].c_str(),"--file")==0){
					file=a[1];
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

		if(zs_strutils::is_empty(file)){
			fprintf(stderr,"Program with arguments you must specify file (i.e --file=filename )\n");
			exit(-1);
		}


		try{
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			try{
				zs->evalFile(file,vm_execute,show_bytecode);
			}catch(std::exception & ex){
				fprintf(stderr,"%s\n",ex.what());
			}
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> time_span=t2-t1;
			printf("executed %s %.0fms\n", zs_path::get_file_name(file).c_str(),time_span.count());
		}catch(std::exception & ex){
			fprintf(stderr,"%s\n",ex.what());
		}
	}
	else{

		bool exit = false;
		std::string expression;
		printf(ZETSCRIP_COPYRIGHT);
		printf("\n");

		do{
			printf("zs>");
			std::getline(std::cin,expression);

			exit = expression=="exit" || expression=="quit";
			if(!exit){ // evaluate expression

				try{
					zs->eval(expression.c_str());
				}catch(std::exception & ex){
					fprintf(stderr,"%s\n",ex.what());
				}
			}

		}while(!exit);// && (instr[++idx_ptr] != NULL));
	}

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
