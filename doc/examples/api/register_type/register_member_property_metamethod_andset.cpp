#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(0xf);\n"
		"Console::outln(\"number.value&=0x7 => {0}\",number.value&=0x7)\n"
	);

	return 0;
}


