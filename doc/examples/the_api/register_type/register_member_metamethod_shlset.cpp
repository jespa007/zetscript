
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number => \"+number)\n"
		"number<<=1;\n"
		"Console::outln(\"number<<=1 => \"+number)\n"
		"number<<=new Number(1);\n"
		"Console::outln(\"number<<=new Number(1) => \"+number)\n"
	);

	return 0;
};


