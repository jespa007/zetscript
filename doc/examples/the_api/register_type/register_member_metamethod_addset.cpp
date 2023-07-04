
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		"var number=new Number(10.5);\n"
		"Console::outln(\"-start:\");\n"
		"Console::outln(number);\n"
		"Console::outln(\"-after addset:\");\n"
		"number=20;\n"
		"Console::outln(number);\n"
	);

	return 0;
};


