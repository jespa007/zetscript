
#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number<<=1 => {0}\",number<<=1)\n"
		"Console::outln(\"number<<=new Number(1) => {0} \",number<<=new Number(1))\n"
	);

	return 0;
}


