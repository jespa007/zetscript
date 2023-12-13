#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);
	
	script_engine.compileAndRun(
		"var number=new Number()\n"
		"if(!number){\n"
		"	Console::outln(\"Number is empty\")	\n"
		"}\n"
	);

	return 0;
}


