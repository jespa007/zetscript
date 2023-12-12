#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value*=2 => {0}\",number.value*=2)\n"
	);

	return 0;
}


