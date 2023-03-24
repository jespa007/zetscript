#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	try{
		zs.evalFile(
			"file.zs"
			,zetscript::EvalOption::EVAL_OPTION_NO_EXECUTE
		);
	}catch(std::exception & _ex){
		fprintf(stderr,"%s\n",_ex.what());
	}
}