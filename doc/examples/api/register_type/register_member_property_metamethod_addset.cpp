#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);
	
	script_engine.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value+=10 => {0}\",number.value+=10)\n"
	);

	return 0;
}


