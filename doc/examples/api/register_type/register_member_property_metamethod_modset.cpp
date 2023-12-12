#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(250);\n"
		"Console::outln(\"number.value%=30 => {0}\",number.value%=30)\n"
	);

	return 0;
}


