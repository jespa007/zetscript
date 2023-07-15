#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"Console::outln(\"new Number(0x1) | new Number(0x2) => \" +  (new Number(0x1) | new Number(0x2)));\n"
		"Console::outln(\"new Number(0x1) | 0x2 => \" + (new Number(0x1) | 0x2));\n"
		"Console::outln(\"0x1 | new Number(0x2) => \" + (0x1 | new Number(0x2)));\n"
	);

	return 0;
};


