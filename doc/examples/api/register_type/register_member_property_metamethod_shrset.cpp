#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number = new Number(0x8);\n"
		"Console::outln(\"number.value>>=1 => {0}\",number.value>>=1)\n"
	);

	return 0;
}


