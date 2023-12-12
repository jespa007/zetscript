#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"Console::outln(\"Result _tostring => \"+new Number(10))"
	);

	return 0;
}


