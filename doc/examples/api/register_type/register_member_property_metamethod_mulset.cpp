#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value*=2 => {0}\",number.value*=2)\n"
	);

	return 0;
}


