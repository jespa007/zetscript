#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"var number=new Number(0xf);\n"
		"Console::outln(\"number.value => \"+number.value)\n"
		"number.value&=0x7;\n"
		"Console::outln(\"number.value&=0x3 => \"+number.value)\n"
		"number.value&=new Number(0x3);\n"
		"Console::outln(\"number.value&=new Number(0x3) => \"+number.value)\n"
	);

	return 0;
};


