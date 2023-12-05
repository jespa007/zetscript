#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(0x1);\n"
		"Console::outln(\"number.value<<=1 => {0}\",number.value<<=1)\n"
	);

	return 0;
}


