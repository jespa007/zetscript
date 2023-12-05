#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"Console::outln(\"new Number(10) <= new Number(20) => \"+(new Number(10) <= new Number(20)))\n"
		"Console::outln(\"new Number(20) <= new Number(20) => \"+(new Number(20) <= new Number(20)))\n"
		"Console::outln(\"new Number(30) <= new Number(20) => \"+(new Number(30) <= new Number(20)))\n"
	);

	return 0;
}


