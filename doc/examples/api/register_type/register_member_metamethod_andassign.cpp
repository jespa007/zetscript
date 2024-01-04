#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(0x7);\n"
		"Console::outln(\"number&=0x3 => {0}\",number&=0x3);\n"
		"Console::outln(\"number&=new Number(0x1) => {0}\",number&=new Number(0x1));\n"
	);

	return 0;
}