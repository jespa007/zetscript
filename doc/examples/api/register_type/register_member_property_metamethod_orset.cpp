#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value|=0x2 => {0}\",number.value|=0x2)\n"
	);

	return 0;
}


