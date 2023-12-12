#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"Console::outln(\"new Number(10) % new Number(20) => \" +  (new Number(10) % new Number(20)));\n"
		"Console::outln(\"new Number(10) % 20 => \" + (new Number(10) % 20));\n"
		"Console::outln(\"10 % new Number(20) => \" + (10 % new Number(20)));\n"
	);
	return 0;
}