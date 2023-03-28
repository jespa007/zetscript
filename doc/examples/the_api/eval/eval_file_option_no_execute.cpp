#include "zetscript.h"
int main(){

	zetscript::ZetScript zs;

	zs.evalFile(
		"file.zs"
		,zetscript::EvalOption::EVAL_OPTION_NO_EXECUTE
	);
	
}
