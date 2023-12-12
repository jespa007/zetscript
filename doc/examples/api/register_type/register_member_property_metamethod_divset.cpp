#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value/=20 => {0}\",number.value/=20)\n"
	);

	return 0;
}


