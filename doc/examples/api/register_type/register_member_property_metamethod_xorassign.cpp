#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&script_engine);
	
	script_engine.compileAndRun(
		"var number=new Number(0x0);\n"
		"Console::outln(\"number.value^=0xa => {0}\",number.value^=0xa)\n"
	);

	return 0;
}


