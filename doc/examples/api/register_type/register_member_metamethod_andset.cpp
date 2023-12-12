#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval(
		"var number=new Number(0x7);\n"
		"Console::outln(\"number&=0x3 => {0}\",number&=0x3);\n"
		"Console::outln(\"number&=new Number(0x1) => {0}\",number&=new Number(0x1));\n"
	);

	return 0;
}