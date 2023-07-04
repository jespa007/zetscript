
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		"var number=new Number(0);\n"
		"Console::outln(\"number => \"+number)\n"
		"number^=0xa;\n"
		"Console::outln(\"number^=0xa => \"+number)\n"
		"number^=new Number(0x9);\n"
		"Console::outln(\"number^=new Number(0x9) => \"+number)\n"
	);

	return 0;
};


