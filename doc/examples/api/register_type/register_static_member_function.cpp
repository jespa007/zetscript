#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"Console::outln(\"Number::pow(2,10) : \"+Number::pow(2,10))"
	);

	return 0;
}


