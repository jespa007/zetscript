#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"var number=new Number(250);\n"
		"Console::outln(\"number.value%=30 => {0}\",number.value%=30)\n"
	);

	return 0;
}


