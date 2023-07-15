#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"Console::outln(\"new Number(20) == new Number(20) => \"+(new Number(20) == new Number(20)))\n"
		"Console::outln(\"new Number(20) == new Number(30) => \"+(new Number(20) == new Number(30)))\n"
	);

	return 0;
};


