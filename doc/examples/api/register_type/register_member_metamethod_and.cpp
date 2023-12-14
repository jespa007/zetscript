#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"Console::outln(\"new Number(0x7) & new Number(0x4) => \" +  (new Number(0x7) & new Number(0x04)));\n"
		"Console::outln(\"new Number(0x7) & 0x4 => \" + (new Number(0x7) & 0x04));\n"
		"Console::outln(\"0x7 & new Number(0x4) => \" + (0x7 & new Number(0x04)));\n"
	);

	return 0;
}