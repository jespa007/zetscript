#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(10.5);\n"
		"Console::outln(\"number.toInteger() : \"+number.toInteger())\n"
	);

	return 0;
}


