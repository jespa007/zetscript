#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);
	
	script_engine.eval(
		"var number=new Number(0x0);\n"
		"Console::outln(\"number.value^=0xa => {0}\",number.value^=0xa)\n"
	);

	return 0;
}


