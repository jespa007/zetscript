#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number = new Number(0x8);\n"
		"Console::outln(\"number.value => \"+number.value)\n"
		"number.value>>=1;\n"
		"Console::outln(\"number.value>>=1 => \"+number.value)\n"
		"number.value>>=new Number(1);\n"
		"Console::outln(\"number.value>>=new Number(1) => \"+number.value)\n"
	);

	return 0;
};


