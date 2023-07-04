#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"On operation 'number=new Number(20)' then 'number.value' is => \"+number.value)\n"
		"number.value = 10;\n"
		"Console::outln(\"On operation 'number=10' the 'number.value' is => \"+number.value)\n"
	);

	return 0;
};


