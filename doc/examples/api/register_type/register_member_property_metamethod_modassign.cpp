#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(250);\n"
		"Console::outln(\"number.value%=30 => {0}\",number.value%=30)\n"
	);

	return 0;
}


