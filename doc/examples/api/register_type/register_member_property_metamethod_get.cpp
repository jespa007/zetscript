#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value => {0}\",number.value)\n"
	);

	return 0;
}


