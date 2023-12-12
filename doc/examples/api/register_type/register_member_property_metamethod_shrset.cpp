#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number = new Number(0x8);\n"
		"Console::outln(\"number.value>>=1 => {0}\",number.value>>=1)\n"
	);

	return 0;
}


