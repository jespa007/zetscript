#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"Console::outln(\"new Number(20) > new Number(20) => \"+(new Number(20) > new Number(20)))\n"
		"Console::outln(\"new Number(40) > new Number(30) => \"+(new Number(40) > new Number(30)))\n"
	);

	return 0;
}


