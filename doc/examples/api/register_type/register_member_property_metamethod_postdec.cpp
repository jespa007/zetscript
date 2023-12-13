#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value-- => {0}\",number.value--)\n"
	);

	return 0;
}


