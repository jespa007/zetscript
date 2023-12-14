#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(30);\n"
		"Console::outln(\"number.value-=20 => {0}\",number.value-=20)\n"
	);

	return 0;
}


