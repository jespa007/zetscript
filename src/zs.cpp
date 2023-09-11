/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include 		"zetscript.h"
//#include        <iostream> //--> loads 244KB
#define ZETSCRIP_COPYRIGHT "ZetScript %i.%i.%i Copyright (C) 2016-2023 Jordi Espada\n",ZETSCRIPT_VERSION_MAJOR,ZETSCRIPT_VERSION_MINOR,ZETSCRIPT_VERSION_PATCH

void show_usage(){

	printf(
	"usage: zs [options] [script]\n"
	"Available options:\n"
	"  --print-byte-code		Prints generated Bytecode\n"
	"  --no-execute			Evaluates script but no execute\n"
	"  --version       		Shows ZetScript version\n"
	"  --no-execution-time		Doesn't print execution time\n"
	);
}


int main(int argc, char * argv[]) {

	zetscript::ZetScript zs;
	const char *param_script_filename="";
	unsigned short eval_options=0;
	bool no_execution_time=false;

	if (argc > 1) {

		int idx_arg=1;
		bool exit_loop=false;
		for(;idx_arg  < argc && exit_loop==false; idx_arg++){

			bool is_option=zetscript::zs_strutils::starts_with(argv[idx_arg],"--"); // is option

			if(is_option){

				if(strcmp(argv[idx_arg],"--no-execute")==0){
					eval_options |= zetscript::EvalOption::ZS_EVAL_OPTION_NO_EXECUTE;
				}else if(strcmp(argv[idx_arg],"--print-byte-code")==0){
					eval_options|=zetscript::EvalOption::ZS_EVAL_OPTION_PRINT_BYTE_CODE;
				}else if(strcmp(argv[idx_arg],"--print-byte-code-all")==0){
					eval_options|=(zetscript::EvalOption::ZS_EVAL_OPTION_PRINT_BYTE_CODE|zetscript::EvalOption::ZS_EVAL_OPTION_PRINT_ALL_BYTE_CODE);
				}else if(strcmp(argv[idx_arg],"--print-byte-code-system")==0){
					eval_options|=zetscript::EvalOption::ZS_EVAL_OPTION_PRINT_ALL_BYTE_CODE;
				}else if(strcmp(argv[idx_arg],"--no-execution-time")==0){
					no_execution_time=true;
				}else if(strcmp(argv[idx_arg],"--version")==0){
					printf(ZETSCRIP_COPYRIGHT);
					goto zs_exit;
				}else{ // unknow option
					fprintf(stderr,"Unrecognized option '%s'\n",argv[idx_arg]);
					show_usage();
					goto zs_exit;
				}
			}else{
				param_script_filename =argv[idx_arg];
				break;
			}
		}
	}


	// if file is empty run interactive console
	if(*param_script_filename ==0){

		bool exit = false;
		char *expression=NULL;
		size_t expression_len=0;
		int readed_bytes=0;
		printf(ZETSCRIP_COPYRIGHT);
		printf("\n");


		do{
			printf("zs>");
			readed_bytes=zetscript::zs_io::getline(&expression,&expression_len,stdin);

			if(readed_bytes==-1){
				continue;
			}

			if(ZS_STRCMP(expression,==,"clear")){
				printf("Clearing symbols...\n");
				zs.clear();
				continue;
			}

			if(ZS_STRCMP(expression,==,"save")){
				printf("Saving state...\n");
				zs.saveState();
				continue;
			}

			exit = ZS_STRCMP(expression,==,"exit") || ZS_STRCMP(expression,==,"quit");

			if(!exit){ // evaluate expression

				try{
					zs.eval(expression);
				}catch(zetscript::zs_exception & ex){
					fprintf(stderr,"[line %i] %s\n",ex.getLine(),ex.what());
				}
			}

			expression_len=0;
			if(expression != NULL){
				free(expression);
				expression=NULL;
			}

		}while(!exit);// && (instr[++idx_ptr] != NULL));

	}else{ // eval script file

		if(zetscript::zs_file::exists(param_script_filename)){

			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

			try{
				zs.evalFile(param_script_filename,eval_options,NULL,__FILE__,__LINE__);
			}catch(zetscript::zs_exception & ex){
				zetscript::zs_string filename=ex.getFilename();
				int line=ex.getLine();
				if(filename !="" && line!=-1){
					fprintf(stderr,"[%s:%i] %s\n", filename.c_str(),line,ex.what());
				}else if(line!=-1){
					fprintf(stderr,"[%i] %s\n",line,ex.what());
				}else{
					fprintf(stderr,"%s\n",ex.what());
				}
			}

			if(no_execution_time==false){
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> time_span=t2-t1;
				printf("executed %s %.0fms\n", zetscript::zs_path::get_filename(param_script_filename).c_str(),time_span.count());
			}

		}else{
			fprintf(stderr,"file '%s' not exits\n", param_script_filename);
		}
	}

zs_exit:

	return 0;
}
