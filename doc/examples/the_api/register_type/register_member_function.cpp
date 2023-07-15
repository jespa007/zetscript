#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(10.5);\n"
		"Console::outln(\"number.toInteger() : \"+number.toInteger())\n"
	);

	return 0;
};


