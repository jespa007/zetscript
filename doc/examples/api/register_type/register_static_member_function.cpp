#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"Console::outln(\"Number::pow(2,10) : \"+Number::pow(2,10))"
	);

	return 0;
}


