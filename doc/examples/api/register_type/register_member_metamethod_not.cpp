#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);
	
	zs.compileAndRun(
		"var number=new Number()\n"
		"if(!number){\n"
		"	Console::outln(\"Number is empty\")	\n"
		"}\n"
	);

	return 0;
}


