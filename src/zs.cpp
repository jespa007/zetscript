/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include 		"zetscript.h"
//#include        <iostream> //--> loads 244KB
#define ZETSCRIP_COPYRIGHT "ScriptEngine %i.%i.%i Copyright (C) 2016-2023 Jordi Espada\n",ZETSCRIPT_VERSION_MAJOR,ZETSCRIPT_VERSION_MINOR,ZETSCRIPT_VERSION_PATCH

void show_usage(){

	printf(
	"usage: zs [options] [script]\n"
	"Available options:\n"
	"  --print-byte-code		Prints generated Bytecode\n"
	"  --no-execute			Evaluates script but no execute\n"
	"  --version       		Shows ScriptEngine version\n"
	"  --no-execution-time		Doesn't print execution time\n"
	);
}

typedef enum:uint16_t{
	 OPTION_NO_EXECUTE			=0x1
	,OPTION_PRINT_BYTE_CODE		=0x2
	,OPTION_PRINT_ALL_BYTE_CODE	=0x4
}Option;

int main(int argc, char * argv[]) {


	uint16_t options=0;
	zetscript::ScriptEngine script_engine;
	const char *param_script_filename="";
	bool no_execution_time=false;

	if (argc > 1) {

		int idx_arg=1;
		bool exit_loop=false;
		zetscript::String str_arg;
		for(;idx_arg  < argc && exit_loop==false; idx_arg++){
			str_arg=argv[idx_arg];
			bool is_option=str_arg.startsWith("--"); // is option

			if(is_option){

				if(strcmp(argv[idx_arg],"--no-execute")==0){
					options |= OPTION_NO_EXECUTE;
				}else if(strcmp(argv[idx_arg],"--print-byte-code")==0){
					options|=OPTION_PRINT_BYTE_CODE;
				}else if(strcmp(argv[idx_arg],"--print-byte-code-all")==0){
					options|=(OPTION_PRINT_BYTE_CODE|OPTION_PRINT_ALL_BYTE_CODE);
				}else if(strcmp(argv[idx_arg],"--print-byte-code-system")==0){
					options|=OPTION_PRINT_ALL_BYTE_CODE;
				}else if(strcmp(argv[idx_arg],"--no-execution-time")==0){
					no_execution_time=true;
				}else if(strcmp(argv[idx_arg],"--version")==0){
					printf(ZETSCRIP_COPYRIGHT);
					return 0;
				}else{ // unknow option
					fprintf(stderr,"Unrecognized option '%s'\n",argv[idx_arg]);
					show_usage();
					return 0;
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
			readed_bytes=zetscript::Console::getline(&expression,&expression_len,stdin);

			if(readed_bytes==-1){
				continue;
			}

			if(ZS_STRCMP(expression,==,"clear")){
				printf("Clearing symbols...\n");
				script_engine.clear();
				continue;
			}

			if(ZS_STRCMP(expression,==,"save")){
				printf("Saving state...\n");
				script_engine.saveState();
				continue;
			}

			exit = ZS_STRCMP(expression,==,"exit") || ZS_STRCMP(expression,==,"quit");

			if(!exit){ // evaluate expression

				try{
					script_engine.eval(expression);
				}catch(zetscript::Exception & ex){
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

		if(zetscript::File::exists(param_script_filename)){

			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

			try{
				script_engine.compileFile(param_script_filename);
				if(options & OPTION_PRINT_BYTE_CODE){
					script_engine.printGeneratedCode();
				}
				if((options & OPTION_NO_EXECUTE) == false){
					script_engine.run();
				}

			}catch(zetscript::Exception & ex){
				zetscript::String filename=ex.getFilename();
				int line=ex.getLine();
				if(filename !="" && line!=-1){
					fprintf(stderr,ZS_FORMAT_FILE_LINE" %s\n", filename.toConstChar(),line,ex.what());
				}else if(line!=-1){
					fprintf(stderr,ZS_FORMAT_LINE" %s\n",line,ex.what());
				}else{
					fprintf(stderr,"%s\n",ex.what());
				}
			}

			if(no_execution_time==false){
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> time_span=t2-t1;
				printf("executed %s %.0fms\n", zetscript::Path::getFilename(param_script_filename).toConstChar(),time_span.count());
			}

		}else{
			fprintf(stderr,"file '%s' not exists\n", param_script_filename);
		}
	}

}
