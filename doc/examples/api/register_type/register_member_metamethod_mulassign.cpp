#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(2);\n"
		"Console::outln(\"number*=2 => {0}\",number*=2)\n"
		"Console::outln(\"number*=new Number(2) => {0}\",number*=new Number(2))\n"
	);
	return 0;
}


