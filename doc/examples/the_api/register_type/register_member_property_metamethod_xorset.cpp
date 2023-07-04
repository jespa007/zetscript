#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"var number=new Number(0x0);\n"
		"Console::outln(\"number.value => \"+number.value)\n"
		"number.value^=0xa;\n"
		"Console::outln(\"number.value^=0xa => \"+number.value)\n"
		"number.value^=new Number(0x9);\n"
		"Console::outln(\"number.value^=new Number(0x9) => \"+number.value)\n"
	);

	return 0;
};


