#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value++ => \"+(number.value++))\n"
		"Console::outln(\"number.value => \"+(number.value))\n"
	);

	return 0;
};


