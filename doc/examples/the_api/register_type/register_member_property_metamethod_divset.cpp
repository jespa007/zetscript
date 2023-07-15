#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value => \"+number.value)\n"
		"number.value/=20;\n"
		"Console::outln(\"number.value/=20 => \"+number.value)\n"
		"number.value/=new Number(30);\n"
		"Console::outln(\"number.value/=new Number(30) => \"+number.value)\n"
	);

	return 0;
};


