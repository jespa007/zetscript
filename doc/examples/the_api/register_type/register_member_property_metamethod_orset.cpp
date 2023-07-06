#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value => \"+number.value)\n"
		"number.value|=0x2;\n"
		"Console::outln(\"number.value|=0x2 => \"+number.value)\n"
		"number.value|=new Number(0x4);\n"
		"Console::outln(\"number.value|=new Number(0x4) => \"+number.value)\n"
	);

	return 0;
};


