
#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

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


