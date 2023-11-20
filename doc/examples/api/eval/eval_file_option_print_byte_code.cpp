#include "zetscript.h"
int main(){

	zetscript::ZetScript zs;

	zs.evalFile(
		"file.zs"
		,ZS_EVAL_OPTION_PRINT_BYTE_CODE
	);
}
