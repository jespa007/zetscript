#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"var number=new Number(10.5);\n"
		"Console::outln(\"number.toInteger() : \"+number.toInteger())\n"
	);

	return 0;
}


