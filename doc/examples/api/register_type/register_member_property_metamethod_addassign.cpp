#include "NumberZs.h"

int main(){

	zetscript::ScriptEngine script_engine;

	NumberZs_register(&script_engine);
	
	script_engine.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value+=10 => {0}\",number.value+=10)\n"
	);

	return 0;
}


