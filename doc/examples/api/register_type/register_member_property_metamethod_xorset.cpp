#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);
	
	se.compileAndRun(
		"var number=new Number(0x0);\n"
		"Console::outln(\"number.value^=0xa => {0}\",number.value^=0xa)\n"
	);

	return 0;
}


