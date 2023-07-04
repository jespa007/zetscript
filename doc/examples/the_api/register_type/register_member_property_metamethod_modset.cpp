#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"var number=new Number(250);\n"
		"Console::outln(\"number.value => \"+number.value)\n"
		"number.value%=30;\n"
		"Console::outln(\"number.value%=30 => \"+number.value)\n"
		"number.value%=new Number(100);\n"
		"Console::outln(\"number.value%=new Number(100) => \"+number.value)\n"
	);

	return 0;
};


