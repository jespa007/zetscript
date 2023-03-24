#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	zs.evalFile(
		"file.zs"
		,zetscript::EvalOption::EVAL_OPTION_PRINT_BYTE_CODE
	);
}