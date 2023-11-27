#include "zetscript.h"
int main(){

	zetscript::ZetScript zs;

	zs.evalFile(
		"file.zs"
		,EVAL_OPTION_PRINT_BYTE_CODE
	);
}
