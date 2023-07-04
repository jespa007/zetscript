#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"Console::outln(\"new Number(0x7) & new Number(0x4) => \" +  (new Number(0x7) & new Number(0x04)));\n"
		"Console::outln(\"new Number(0x7) & 0x4 => \" + (new Number(0x7) & 0x04));\n"
		"Console::outln(\"0x7 & new Number(0x4) => \" + (0x7 & new Number(0x04)));\n"
	);

	return 0;
};


