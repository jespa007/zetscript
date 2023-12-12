#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(10.5);\n"
		"Console::outln(\"number.toInteger() : \"+number.toInteger())\n"
	);

	return 0;
}


