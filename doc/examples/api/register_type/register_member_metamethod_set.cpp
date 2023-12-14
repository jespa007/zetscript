#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var n=new Number(0);\n"
		"Console::outln(\"n=10 => {0}\",n=10)\n"
		"Console::outln(\"n=new Number(20) => {0}\",n=new Number(20))\n"
	);

	return 0;
}


