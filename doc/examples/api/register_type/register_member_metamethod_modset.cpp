#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval(
		"var number=new Number(250);\n"
		"Console::outln(\"number%=30 => {0}\",number%=30)\n"
		"Console::outln(\"number%=new Number(100) => {0}\",number%=new Number(100))\n"
	);

	return 0;
}


