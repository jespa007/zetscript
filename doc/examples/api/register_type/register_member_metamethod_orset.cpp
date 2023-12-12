#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number|=0x2 => {0}\",number|=0x2)\n"
		"Console::outln(\"number|=new Number(0x4) => {0}\",number|=new Number(0x4))\n"
	);

	return 0;
}


