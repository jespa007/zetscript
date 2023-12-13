#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"var number = new Number(0x8);\n"
		"Console::outln(\"number.value>>=1 => {0}\",number.value>>=1)\n"
	);

	return 0;
}


