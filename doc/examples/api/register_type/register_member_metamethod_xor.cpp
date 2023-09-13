#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"Console::outln(\"new Number(0xa) ^ new Number(0x9) => \" +  (new Number(0xa) ^ new Number(0x9)));\n"
		"Console::outln(\"new Number(0xa) ^ 0x9 => \" + (new Number(0xa) ^ 0x9));\n"
		"Console::outln(\"0xa ^ new Number(0x9) => \" + (0xa ^ new Number(0x9)));\n"
	);

	return 0;
}

