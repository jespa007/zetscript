#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval(
		"Console::outln(\"Result _tostring => \"+new Number(10))"
	);

	return 0;
}


