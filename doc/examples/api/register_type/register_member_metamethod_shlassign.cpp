
#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);

	script_engine.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number<<=1 => {0}\",number<<=1)\n"
		"Console::outln(\"number<<=new Number(1) => {0} \",number<<=new Number(1))\n"
	);

	return 0;
}


