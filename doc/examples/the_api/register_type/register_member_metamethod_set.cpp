#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var n=new Number ();\n"
		"Console::outln(\"n => \"+n)\n"
		"n=10;\n"
		"Console::outln(\"n=10 => \"+n)\n"
		"n=new Number(20);\n"
		"Console::outln(\"n=new Number(20) => \"+n)\n"
	);

	return 0;
};


