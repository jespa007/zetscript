/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include 		"zetscript.h"
//#include        <iostream> //--> loads 244KB

#define ZETSCRIP_COPYRIGHT "ZetScript %i.%i.%i Copyright (C) 2016-2021 Jordi Espada\n",ZETSCRIPT_VERSION_MAJOR,ZETSCRIPT_VERSION_MINOR,ZETSCRIPT_VERSION_PATCH


using namespace zetscript;

void show_usage(){

	printf(
	"usage: zs [options] [script]\n"
	"Available options:\n"
	"  --show-bc       	    Shows the Bytecode generated\n"
	"  --no-execute			Compiles the script but no execute\n"
	"  --version       		Shows ZetScript version\n"
	);
}


int main(int argc, char * argv[]) {

	ZetScript *zs = new ZetScript();
	const char *file="";
	unsigned short eval_options=0;

	if (argc > 1) {


		int idx_arg=1;
		bool exit_loop=false;
		for(;idx_arg  < argc && exit_loop==false; idx_arg++){

			bool is_option=zs_strutils::starts_with(argv[idx_arg],"--"); // is option

			if(is_option){

				if(strcmp(argv[idx_arg],"--no-execute")==0){
					eval_options |= zetscript::EvalOption::EVAL_OPTION_NO_EXECUTE;
				}else if(strcmp(argv[idx_arg],"--show-bc")==0){
					eval_options|=zetscript::EvalOption::EVAL_OPTION_SHOW_USER_BYTE_CODE;
				}else if(strcmp(argv[idx_arg],"--show-bc-all")==0){
					eval_options|=(zetscript::EvalOption::EVAL_OPTION_SHOW_USER_BYTE_CODE|zetscript::EvalOption::EVAL_OPTION_SHOW_SYSTEM_BYTE_CODE);
				}else if(strcmp(argv[idx_arg],"--show-bc-system")==0){
					eval_options|=zetscript::EvalOption::EVAL_OPTION_SHOW_SYSTEM_BYTE_CODE;
				}else if(strcmp(argv[idx_arg],"--version")==0){
					printf(ZETSCRIP_COPYRIGHT);
					exit(0);
				}else{ // unknow option
					fprintf(stderr,"Unrecognized option '%s'\n",argv[idx_arg]);
					show_usage();
					exit(EXIT_FAILURE);
				}
			}else{
				file=argv[idx_arg];
				break;
			}
		}
	}


	// if file is empty run interactive console
	if(*file==0){

		bool exit = false;
		char *expression=NULL;
		size_t expression_len=0;
		int readed_bytes=0;
		printf(ZETSCRIP_COPYRIGHT);
		printf("\n");


		do{
			printf("zs>");
			readed_bytes=zs_io::getline(&expression,&expression_len,stdin);

			if(readed_bytes==-1){
				continue;
			}

			if(ZS_STRCMP(expression,==,"clear")){
				printf("Clearing symbols...\n");
				zs->clear();
				continue;
			}

			if(ZS_STRCMP(expression,==,"save")){
				printf("Saving state...\n");
				zs->saveState();
				continue;
			}

			exit = ZS_STRCMP(expression,==,"exit") || ZS_STRCMP(expression,==,"quit");

			if(!exit){ // evaluate expression

				try{
					zs->eval(expression);
				}catch(std::exception & ex){
					fprintf(stderr,"%s\n",ex.what());
				}
			}

			expression_len=0;
			if(expression != NULL){
				free(expression);
				expression=NULL;
			}

		}while(!exit);// && (instr[++idx_ptr] != NULL));

	}else{ // eval script file

		if(zs_file::exists(file)){

			try{
				std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
				try{
					zs->evalFile(file,eval_options);
				}catch(std::exception & ex){
					fprintf(stderr,"%s\n",ex.what());
				}
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> time_span=t2-t1;
				printf("executed %s %.0fms\n", zs_path::get_filename(file).c_str(),time_span.count());
			}catch(std::exception & ex){
				fprintf(stderr,"%s\n",ex.what());
			}
		}else{
			fprintf(stderr,"file '%s' not exits\n",file);
		}
	}

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
