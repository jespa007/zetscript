#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value<<=1 => {0}\",number.value<<=1)\n"
	);

	return 0;
}


