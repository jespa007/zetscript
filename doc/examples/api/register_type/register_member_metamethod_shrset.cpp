
#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(0x10);\n"
		"Console::outln(\"number>>=1 => {0}\",number>>=1)\n"
		"Console::outln(\"number>>=new Number(1) => {0}\",number>>=new Number(1))\n"
	);

	return 0;
}


