#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"Console::outln(\"new Number(0x8) >> new Number(2) => \" +  (new Number(0x8) >> new Number(2)));\n"
		"Console::outln(\"new Number(0x8) >> 2 => \" + (new Number(0x8) >> 2));\n"
		"Console::outln(\"0x8 >> new Number(2) => \" + (0x8 >> new Number(2)));\n"
	);

	return 0;
}


