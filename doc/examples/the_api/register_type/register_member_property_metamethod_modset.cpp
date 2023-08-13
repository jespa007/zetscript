#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(250);\n"
		"Console::outln(\"number.value%=30 => {0}\",number.value%=30)\n"
	);

	return 0;
}


