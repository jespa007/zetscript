#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"++number.value => {0}\",++number.value)\n"
	);

	return 0;
}


