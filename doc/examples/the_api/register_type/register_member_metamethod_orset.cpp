#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number|=0x2 => {0}\",number|=0x2)\n"
		"Console::outln(\"number|=new Number(0x4) => {0}\",number|=new Number(0x4))\n"
	);

	return 0;
}


