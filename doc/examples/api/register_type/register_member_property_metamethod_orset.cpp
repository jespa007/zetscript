#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value|=0x2 => {0}\",number.value|=0x2)\n"
	);

	return 0;
}


