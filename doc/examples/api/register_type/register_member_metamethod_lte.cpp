#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"Console::outln(\"new Number(10) <= new Number(20) => \"+(new Number(10) <= new Number(20)))\n"
		"Console::outln(\"new Number(20) <= new Number(20) => \"+(new Number(20) <= new Number(20)))\n"
		"Console::outln(\"new Number(30) <= new Number(20) => \"+(new Number(30) <= new Number(20)))\n"
	);

	return 0;
}


