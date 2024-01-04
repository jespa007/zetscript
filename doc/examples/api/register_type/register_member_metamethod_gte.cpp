#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"Console::outln(\"new Number(10) >= new Number(20) => \"+(new Number(10) >= new Number(20)))\n"
		"Console::outln(\"new Number(20) >= new Number(20) => \"+(new Number(20) >= new Number(20)))\n"
		"Console::outln(\"new Number(30) >= new Number(20) => \"+(new Number(30) >= new Number(20)))\n"
	);

	return 0;
}


