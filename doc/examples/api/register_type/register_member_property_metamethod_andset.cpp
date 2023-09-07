#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(0xf);\n"
		"Console::outln(\"number.value&=0x7 => {0}\",number.value&=0x7)\n"
	);

	return 0;
}


