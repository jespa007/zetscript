#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value|=0x2 => {0}\",number.value|=0x2)\n"
	);

	return 0;
}


