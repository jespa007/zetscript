#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value*=2 => {0}\",number.value*=2)\n"
	);

	return 0;
}


