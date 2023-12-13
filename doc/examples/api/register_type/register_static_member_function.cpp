#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"Console::outln(\"Number::pow(2,10) : \"+Number::pow(2,10))"
	);

	return 0;
}


