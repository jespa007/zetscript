#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value/=20 => {0}\"+number.value/=20)\n"
	);

	return 0;
}


