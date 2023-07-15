
#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(250);\n"
		"Console::outln(\"number => \"+number)\n"
		"number%=30;\n"
		"Console::outln(\"number%=30 => \"+number)\n"
		"number%=new Number(100);\n"
		"Console::outln(\"number%=new Number(100) => \"+number)\n"
	);

	return 0;
};


