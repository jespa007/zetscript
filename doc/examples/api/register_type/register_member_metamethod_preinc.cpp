
#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"++number => {0}\",++number)\n"
	);

	return 0;
}


