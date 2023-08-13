#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value|=0x2 => {0}\",number.value|=0x2)\n"
	);

	return 0;
}


