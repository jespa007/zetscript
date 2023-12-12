#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);
	
	se.compileAndRun(
		"var number=new Number()\n"
		"if(!number){\n"
		"	Console::outln(\"Number is empty\")	\n"
		"}\n"
	);

	return 0;
}


