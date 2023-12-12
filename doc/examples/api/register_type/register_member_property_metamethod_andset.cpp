#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval(
		"var number=new Number(0xf);\n"
		"Console::outln(\"number.value&=0x7 => {0}\",number.value&=0x7)\n"
	);

	return 0;
}


