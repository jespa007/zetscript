#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(0xf);\n"
		"Console::outln(\"number.value&=0x7 => {0}\",number.value&=0x7)\n"
	);

	return 0;
}


