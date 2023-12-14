#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number();\n"
		"Console::outln(\"number::MAX_VALUE => {0}\",number::MAX_VALUE)\n"
	);

	return 0;
}


