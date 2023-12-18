#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value*=2 => {0}\",number.value*=2)\n"
	);

	return 0;
}


