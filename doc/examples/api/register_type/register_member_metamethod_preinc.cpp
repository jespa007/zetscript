
#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"++number => {0}\",++number)\n"
	);

	return 0;
}


