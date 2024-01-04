#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(10.5);\n"
		"Console::outln(\"number.toInteger() : \"+number.toInteger())\n"
	);

	return 0;
}


